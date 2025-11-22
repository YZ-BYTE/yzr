#include "ckernal.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>

#include "mainwindow.h"

#include <iostream>
#include <cstring>
#include <string>

#include "TcpClientMediator.h"
#include "TcpServerMediator.h"
#include "netapi/net/packdef.h"
extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

CKernal &CKernal::getInstance()
{
    static CKernal instance;
    return instance;
}

void CKernal::initialize(int &argc, char **argv)
{
    if (m_initialized)
    {
        return;
    }

    m_app.reset(new QApplication(argc, argv));
    initFFmpeg();
    loadIniFile();

    m_mainWindow.reset(new MainWindow());

    QObject::connect(
        m_mainWindow.get(),
        &MainWindow::requestClose,
        this,
        &CKernal::onMainWindowRequestClose);

    startNetwork();

    m_initialized = true;
}

int CKernal::run()
{
    if (!m_initialized || !m_app)
    {
        std::cerr << "CKernal 尚未初始化" << std::endl;
        return -1;
    }

    if (m_mainWindow)
    {
        m_mainWindow->show();
    }

    return m_app->exec();
}

CKernal::CKernal()
    : QObject(nullptr),
      m_app(nullptr),
      m_mainWindow(nullptr),
      m_initialized(false),
      m_tcpClient(nullptr),
      m_tcpServer(nullptr)
{
}

CKernal::~CKernal()
{
    if (m_tcpClient)
    {
        m_tcpClient->CloseNet();
        delete m_tcpClient;
        m_tcpClient = nullptr;
    }

    if (m_tcpServer)
    {
        m_tcpServer->CloseNet();
        delete m_tcpServer;
        m_tcpServer = nullptr;
    }
}

void CKernal::initFFmpeg()
{
    std::cout << "Hello FFmpeg!" << std::endl;
    av_register_all();
    unsigned version = avcodec_version();
    std::cout << "version is:" << version << std::endl;
}

void CKernal::onMainWindowRequestClose()
{
    cleanup();
}

void CKernal::cleanup()
{
    if (m_mainWindow)
    {
        m_mainWindow->hide();
        m_mainWindow.reset();
    }

    if (m_app)
    {
        m_app->quit();
    }

    m_initialized = false;
}

void CKernal::loadIniFile()
{
    const QString defaultIp = "160.168.233.131";
    const QString defaultPort = "8000";

    m_ip = defaultIp;
    m_port = defaultPort;

    const QString path = QCoreApplication::applicationDirPath() + "/config.ini";
    QFileInfo info(path);

    QSettings setting(path, QSettings::IniFormat);

    if (info.exists())
    {
        setting.beginGroup("network");
        const QString fileIp = setting.value("ip").toString();
        const QString filePort = setting.value("port").toString();
        setting.endGroup();

        if (fileIp == defaultIp)
        {
            m_ip = fileIp;
        }
        if (filePort == defaultPort)
        {
            m_port = filePort;
        }
    }

    setting.beginGroup("network");
    setting.setValue("ip", defaultIp);
    setting.setValue("port", defaultPort);
    setting.endGroup();
    setting.sync();
}

void CKernal::startNetwork()
{
    if (!m_tcpClient)
    {
        m_tcpClient = new TcpClientMediator;
        QObject::connect(
            m_tcpClient,
            &INetMediator::SIG_ReadyData,
            this,
            &CKernal::slotDealClientData);
    }

    bool ok = false;
    unsigned short port = static_cast<unsigned short>(m_port.toUShort(&ok));
    if (!ok)
    {
        port = _DEF_TCP_PORT;
        qWarning() << "配置端口无效，使用默认端口" << port;
    }

    const std::string ip = m_ip.toStdString();
    if (!ip.empty())
    {
        // OpenNet现在只保存配置，不立即连接（延迟连接机制）
        if (m_tcpClient->OpenNet(ip.c_str(), port))
        {
            qInfo() << "[CKernal] Network configured, will connect on demand. IP:" << QString::fromStdString(ip) << "Port:" << port;
        }
        else
        {
            qWarning() << "配置网络失败:" << QString::fromStdString(ip) << port;
        }
    }
}

// 客户端处理数据
void CKernal::slotDealClientData(unsigned int lSendIP, char *buf, int nlen)
{
    Q_UNUSED(lSendIP);
    
    // 检查包类型，如果是登录/注册/上传相关的包，不在这里处理（由各自的对话框处理）
    if (nlen >= (int)sizeof(int))
    {
        int packType = *(int*)buf;
        if (packType == _DEF_PACK_LOGIN_RS || 
            packType == _DEF_PACK_REGISTER_RS ||
            packType == _DEF_PACK_UPLOAD_RS)
        {
            // 登录/注册/上传响应包，不在这里处理，也不释放内存（由各自的对话框处理）
            // 这些包会通过SIG_ReadyData信号传递给对应的对话框
            return;
        }
    }
    
    // 其他类型的数据包，回收空间
    delete[] buf;
}


void CKernal::slotDealServerData(unsigned int lSendIP, char *buf, int nlen)
{
    Q_UNUSED(lSendIP);
    Q_UNUSED(nlen);
    
    // 转发数据（如果需要）
    // m_tcpServer->SendData(lSendIP, buf, nlen);
    delete[] buf;
}





























