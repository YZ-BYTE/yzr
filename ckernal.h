#ifndef CKERNAL_H
#define CKERNAL_H

#include <memory>
#include <QObject>
#include <QString>
class QApplication;
class MainWindow;

//#include<INetMediator.h>
class INetMediator;

class CKernal : public QObject
{
    Q_OBJECT
public:
    static CKernal &getInstance();

    void initialize(int &argc, char **argv);
    int run();
    
    // 获取TCP客户端网络模块
    INetMediator* getTcpClient() { return m_tcpClient; }

private:
    CKernal();
    ~CKernal();

    CKernal(const CKernal &) = delete;
    CKernal &operator=(const CKernal &) = delete;

    void initFFmpeg();
    void onMainWindowRequestClose();
    void cleanup();
    void loadIniFile();
    void startNetwork();

    std::unique_ptr<QApplication> m_app;
    std::unique_ptr<MainWindow> m_mainWindow;
    bool m_initialized;

    QString m_ip;
    QString m_port;

    INetMediator *m_tcpClient;
    INetMediator *m_tcpServer;

private slots:
    void slotDealClientData(unsigned int lSendIP, char *buf, int nlen);
    void slotDealServerData(unsigned int lSendIP, char *buf, int nlen);
};

#endif // CKERNAL_H

































