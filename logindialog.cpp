#include "logindialog.h"
#include "ui_logindialog.h"
#include "md5util.h"
#include "netapi/net/packdef.h"
#include <QMessageBox>
#include <QString>
#include <string>
#include <cstring>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_tcpClient(nullptr)  //初始化网络中介为null
{
    ui->setupUi(this);
    
    // 设置窗口标题
    setWindowTitle("登录/注册");
    
    // 默认显示登录标签页
    ui->tabWidget->setCurrentIndex(0);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setTcpClient(INetMediator* tcpClient)
{
    m_tcpClient = tcpClient;
    // 信号连接将在外部统一处理，这里只保存指针
}

void LoginDialog::setCurrentTab(int index)
{
    if (index >= 0 && index < 2)
    {
        ui->tabWidget->setCurrentIndex(index);
    }
}

void LoginDialog::on_pb_login_clicked()
{
    //获取用户输入的用户名和密码 去除前后空格
    QString username = ui->le_username_login->text().trimmed();
    QString password = ui->le_password_login->text();
    
    // 验证输入
    if (username.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请输入用户名");
        return;
    }
    if (password.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请输入密码");
        return;
    }
    //提交登录请求
    slot_loginCommit(username, password);
}

void LoginDialog::on_pb_register_clicked()
{
    //获取用户注册信息
    QString username = ui->le_username_register->text().trimmed();
    QString password = ui->le_password_register->text();
    QString confirmPassword = ui->le_confirm_password->text();
    
    // 验证输入
    if (username.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请输入用户名");
        return;
    }
    if (password.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请输入密码");
        return;
    }
    if (password != confirmPassword)
    {
        QMessageBox::warning(this, "提示", "两次输入的密码不一致");
        return;
    }
    //收集用户选择的兴趣爱好
    Hobby hy = collectHobbies();
    //提交注册请求
    slot_registerCommit(username, password, hy);
}

void LoginDialog::on_pb_clear_clicked()
{
    // 清空所有输入
    ui->le_username_register->clear();
    ui->le_password_register->clear();
    ui->le_confirm_password->clear();
    
    // 清空所有复选框
    ui->cb_food->setChecked(false);
    ui->cb_funny->setChecked(false);
    ui->cb_ennegy->setChecked(false);
    ui->cb_dance->setChecked(false);
    ui->cb_music->setChecked(false);
    ui->cb_video->setChecked(false);
    ui->cb_outside->setChecked(false);
    ui->cb_edu->setChecked(false);
    ui->cb_selectAll->setChecked(false);
    ui->cb_deselectAll->setChecked(false);
}
//全选
void LoginDialog::on_cb_selectAll_clicked()
{
    bool checked = ui->cb_selectAll->isChecked();
    ui->cb_food->setChecked(checked);
    ui->cb_funny->setChecked(checked);
    ui->cb_ennegy->setChecked(checked);
    ui->cb_dance->setChecked(checked);
    ui->cb_music->setChecked(checked);
    ui->cb_video->setChecked(checked);
    ui->cb_outside->setChecked(checked);
    ui->cb_edu->setChecked(checked);
    
    if (checked)
    {
        ui->cb_deselectAll->setChecked(false);
    }
}
//取消全选
void LoginDialog::on_cb_deselectAll_clicked()
{
    bool checked = ui->cb_deselectAll->isChecked();
    if (checked)
    {
        ui->cb_food->setChecked(false);
        ui->cb_funny->setChecked(false);
        ui->cb_ennegy->setChecked(false);
        ui->cb_dance->setChecked(false);
        ui->cb_music->setChecked(false);
        ui->cb_video->setChecked(false);
        ui->cb_outside->setChecked(false);
        ui->cb_edu->setChecked(false);
        ui->cb_selectAll->setChecked(false);
    }
}
//收集用户选择的兴趣爱好
Hobby LoginDialog::collectHobbies()
{
    Hobby hy;
    hy.dance = ui->cb_dance->isChecked() ? 1 : 0;
    hy.edu = ui->cb_edu->isChecked() ? 1 : 0;
    hy.ennegy = ui->cb_ennegy->isChecked() ? 1 : 0;
    hy.food = ui->cb_food->isChecked() ? 1 : 0;
    hy.funny = ui->cb_funny->isChecked() ? 1 : 0;
    hy.music = ui->cb_music->isChecked() ? 1 : 0;
    hy.outside = ui->cb_outside->isChecked() ? 1 : 0;
    hy.video = ui->cb_video->isChecked() ? 1 : 0;
    return hy;
}
//对密码进行MD5加密 封装为登录请求结构体并通过网络发送
void LoginDialog::slot_loginCommit(QString name, QString password)
{
    // 登录提交 MD5
    std::string strName = name.toStdString();
    char* bufName = (char*)strName.c_str();
    
    // MD5加密密码
    QByteArray bt = GetMD5(password);
    
    // 填充登录请求结构体
    STRU_LOGIN_RQ rq;
    strcpy_s(rq.m_szUser, _MAX_SIZE, bufName);
    memcpy(rq.m_szPassword, bt.data(), bt.length());
    
    // 发送数据
    if (!m_tcpClient)
    {
        QMessageBox::warning(this, "提示", "网络模块未初始化");
        return;
    }
    
    if (m_tcpClient->SendData(0, (char*)&rq, sizeof(rq)) < 0)
    {
        QMessageBox::about(this, "提示", "网络故障");
    }
}

void LoginDialog::slot_registerCommit(QString name, QString password, Hobby hy)
{
    // 注册提交
    std::string strName = name.toStdString();
    char* bufName = (char*)strName.c_str();
    
    // MD5加密密码
    QByteArray bt = GetMD5(password);
    
    // 填充注册请求结构体
    STRU_REGISTER_RQ rq;
    strcpy_s(rq.m_szUser, _MAX_SIZE, bufName);
    memcpy(rq.m_szPassword, bt.data(), bt.length());
    
    // 填充喜好字段
    rq.dance = hy.dance;
    rq.edu = hy.edu;
    rq.ennegy = hy.ennegy;
    rq.food = hy.food;
    rq.funny = hy.funny;
    rq.music = hy.music;
    rq.outside = hy.outside;
    rq.video = hy.video;
    
    // 发送数据
    if (!m_tcpClient)
    {
        QMessageBox::warning(this, "提示", "网络模块未初始化");
        return;
    }
    
    if (m_tcpClient->SendData(0, (char*)&rq, sizeof(rq)) < 0)
    {
        QMessageBox::about(this, "提示", "网络故障");
    }
}

//处理服务器返回的登录响应
void LoginDialog::slot_dealLoginRs(unsigned int lSendIP, char* buf, int nlen)
{
    Q_UNUSED(lSendIP);
    
    if (nlen < (int)sizeof(STRU_LOGIN_RS))
        return;
    //将缓冲区数据转换为登录响应结构体
    STRU_LOGIN_RS* rs = (STRU_LOGIN_RS*)buf;
    if (rs->m_nType != _DEF_PACK_LOGIN_RS)
        return;
    
    // 处理登录响应
    if (rs->m_lResult == login_success)
    {
        QMessageBox::information(this, "提示", QString("登录成功！用户ID: %1").arg(rs->m_UserID));
        accept(); // 关闭对话框
    }
    else if (rs->m_lResult == user_not_exist)
    {
        QMessageBox::warning(this, "提示", "用户不存在");
    }
    else if (rs->m_lResult == password_error)
    {
        QMessageBox::warning(this, "提示", "密码错误");
    }
    else
    {
        QMessageBox::warning(this, "提示", "登录失败");
    }
    
    // 释放内存
    delete[] buf;
}

void LoginDialog::slot_dealRegisterRs(unsigned int lSendIP, char* buf, int nlen)
{
    Q_UNUSED(lSendIP);
    
    if (nlen < (int)sizeof(STRU_REGISTER_RS))
        return;
    
    STRU_REGISTER_RS* rs = (STRU_REGISTER_RS*)buf;
    if (rs->m_nType != _DEF_PACK_REGISTER_RS)
        return;
    
    // 处理注册响应
    if (rs->m_lResult == register_success)
    {
        QMessageBox::information(this, "提示", "注册成功！");
        // 切换到登录标签页
        ui->tabWidget->setCurrentIndex(0);
        // 清空注册页面的输入
        on_pb_clear_clicked();
    }
    else if (rs->m_lResult == user_is_exist)
    {
        QMessageBox::warning(this, "提示", "用户已存在");
    }
    else
    {
        QMessageBox::warning(this, "提示", "注册失败");
    }
    
    // 释放内存
    delete[] buf;
}

