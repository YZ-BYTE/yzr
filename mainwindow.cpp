#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playerdialog.h"
#include "recorder/recorderdialog.h"
#include "logindialog.h"
#include "uploadvideodialog.h"
#include "ckernal.h"
#include "netapi/net/packdef.h"
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_userId(0)
{
    ui->setupUi(this);
    statusBar()->showMessage("就绪");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOnlineTV_triggered()
{
    statusBar()->showMessage("正在打开在线电视点播...", 2000);
    PlayerDialog *playerDialog = new PlayerDialog(this);
    playerDialog->setAttribute(Qt::WA_DeleteOnClose);
    playerDialog->setWindowTitle("在线电视点播");
    playerDialog->show();
}

void MainWindow::on_actionLocalFile_triggered()
{
    statusBar()->showMessage("正在打开本地文件播放...", 2000);
    PlayerDialog *playerDialog = new PlayerDialog(this, false);
    playerDialog->setAttribute(Qt::WA_DeleteOnClose);
    playerDialog->setWindowTitle("本地文件播放");
    playerDialog->show();
    playerDialog->triggerOpenFile();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "关于", 
        "媒体播放器 v1.0\n\n"
        "支持本地视频文件播放和在线电视点播功能\n"
        "基于FFmpeg和Qt开发");
}

void MainWindow::on_actionRecorder_triggered()
{
    statusBar()->showMessage("正在打开录制控制...", 2000);
    RecorderDialog *recorderDialog = new RecorderDialog();
    recorderDialog->setAttribute(Qt::WA_DeleteOnClose);
    recorderDialog->show();
}

void MainWindow::on_actionLogin_triggered()
{
    openLoginDialog(0); // 0表示登录标签页
}

void MainWindow::on_pb_login_clicked()
{
    openLoginDialog(0); // 0表示登录标签页
}

void MainWindow::on_pb_register_clicked()
{
    openLoginDialog(1); // 1表示注册标签页
}

void MainWindow::openLoginDialog(int tabIndex)
{
    statusBar()->showMessage("正在打开登录/注册...", 2000);
    LoginDialog *loginDialog = new LoginDialog(this);
    loginDialog->setAttribute(Qt::WA_DeleteOnClose);
    
    // 设置网络模块
    CKernal& kernal = CKernal::getInstance();
    INetMediator* tcpClient = kernal.getTcpClient();
    if (tcpClient)
    {
        loginDialog->setTcpClient(tcpClient);
        // 连接网络响应信号到登录对话框
        connect(tcpClient, &INetMediator::SIG_ReadyData,
                loginDialog, &LoginDialog::slot_dealLoginRs);
        connect(tcpClient, &INetMediator::SIG_ReadyData,
                loginDialog, &LoginDialog::slot_dealRegisterRs);
        
        // 连接登录成功信号到主窗口
        // 注意：这里需要通过信号转发，因为登录对话框没有直接发出用户ID信号
        // 我们需要监听登录响应并提取用户ID
        connect(tcpClient, &INetMediator::SIG_ReadyData,
                this, [this](unsigned int lSendIP, char* buf, int nlen) {
                    if (nlen >= (int)sizeof(STRU_LOGIN_RS))
                    {
                        STRU_LOGIN_RS* rs = (STRU_LOGIN_RS*)buf;
                        if (rs->m_nType == _DEF_PACK_LOGIN_RS && rs->m_lResult == login_success)
                        {
                            handleLoginSuccess(rs->m_UserID);
                        }
                    }
                });
    }
    
    // 设置要显示的标签页
    loginDialog->setCurrentTab(tabIndex);
    
    loginDialog->show();
}

void MainWindow::on_pb_uploadVideo_clicked()
{
    // 检查是否已登录
    if (m_userId <= 0)
    {
        QMessageBox::warning(this, "提示", "请先登录后再上传视频");
        openLoginDialog(0); // 打开登录对话框
        return;
    }
    
    statusBar()->showMessage("正在打开上传视频...", 2000);
    UploadVideoDialog *uploadDialog = new UploadVideoDialog(this);
    uploadDialog->setAttribute(Qt::WA_DeleteOnClose);
    
    // 设置网络模块
    CKernal& kernal = CKernal::getInstance();
    INetMediator* tcpClient = kernal.getTcpClient();
    if (tcpClient)
    {
        uploadDialog->setTcpClient(tcpClient);
        uploadDialog->setUserId(m_userId);
        
        // 连接网络响应信号到上传对话框
        connect(tcpClient, &INetMediator::SIG_ReadyData,
                uploadDialog, &UploadVideoDialog::slot_dealUploadRs);
    }
    
    uploadDialog->show();
}

void MainWindow::handleLoginSuccess(int userId)
{
    m_userId = userId;
    statusBar()->showMessage(QString("登录成功，用户ID: %1").arg(userId), 3000);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton ret = QMessageBox::question(
        this,
        "退出确认",
        "确定要退出应用吗？",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (ret == QMessageBox::Yes)
    {
        emit requestClose();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

