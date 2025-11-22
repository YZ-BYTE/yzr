#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H
#include"videoplayer.h"
#include "channelinfo.h"
#include <QDialog>
#include <QTimer>
#include <QDebug>
#include<QMouseEvent>
#include <QListWidgetItem>
QT_BEGIN_NAMESPACE
namespace Ui { class PlayerDialog; }
QT_END_NAMESPACE



class PlayerDialog : public QDialog
{
    Q_OBJECT

public:
    PlayerDialog(QWidget *parent = nullptr, bool showChannelList = true);
    ~PlayerDialog();
    
    // 控制频道列表显示/隐藏
    void setChannelListVisible(bool visible);
    
    // 触发打开文件对话框（用于从菜单调用）
    void triggerOpenFile();

private slots:
    //void on_pushButton_clicked();

    void on_pb_start_clicked();
    void slot_setImage(QImage img);


    void on_pb_resume_clicked();

    void on_pb_pause_clicked();

    void on_pb_stop_clicked();

    void slot_PlayerStateChanged(int state);
    void slot_TimerTimeOut();
    void slot_getTotalTime(qint64 uSec);
    //事件过滤器
    bool eventFilter(QObject*obj,QEvent* event);
    // 频道相关槽函数
    void on_listWidget_channels_itemDoubleClicked(QListWidgetItem *item);
    void initChannelList();
    void playChannel(const ChannelInfo& channel);
private:

    Ui::PlayerDialog *ui;
    VideoPlayer * m_player;
    QTimer m_timer;

    //停止的状态
    bool isStop;

};
#endif // PLAYERDIALOG_H
