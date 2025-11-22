#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
//引入网络中介者接口
#include "netapi/mediator/INetMediator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

// 喜好结构体
struct Hobby
{
    int dance;      // 舞蹈
    int edu;        // 教育
    int ennegy;     // 正能量
    int food;       // 美食
    int funny;      // 搞笑
    int music;      // 歌曲
    int outside;    // 户外
    int video;      // 影视
};
//负责用户登录，注册功能的UI交互和业务逻辑处理
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    // 设置网络模块
    void setTcpClient(INetMediator* tcpClient);
    
    // 设置当前显示的标签页 (0=登录, 1=注册)
    void setCurrentTab(int index);

public slots:
    // 处理服务器响应（需要从外部连接信号，所以必须是public）
    void slot_dealLoginRs(unsigned int lSendIP, char* buf, int nlen);
    void slot_dealRegisterRs(unsigned int lSendIP, char* buf, int nlen);

private slots:
    // 登录相关
    void on_pb_login_clicked();    //登录按钮
    
    // 注册相关
    void on_pb_register_clicked();//注册按钮
    void on_pb_clear_clicked();//清空注册按钮
    void on_cb_selectAll_clicked();//全选兴趣爱好
    void on_cb_deselectAll_clicked();//取消全选

private:
    Ui::LoginDialog *ui;
    INetMediator* m_tcpClient;
    
    // 收集注册页面的喜好选择
    Hobby collectHobbies();
    
    // 登录提交
    void slot_loginCommit(QString name, QString password);
    
    // 注册提交
    void slot_registerCommit(QString name, QString password, Hobby hy);
};

#endif // LOGINDIALOG_H

