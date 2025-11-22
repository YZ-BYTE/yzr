#include "playerdialog.h"
#include "ui_playerdialog.h"

//#define _DEF_PATH "rtmp://160.168.233.131:1935/vod//1/101.mp4"
#define _DEF_LIVE_PATH "http://111.40.196.9/PLTV/88888888/224/3221225488/index.m3u8"
//#define _DEF_PATH "http://160.168.233.131:80/hls/output.m3u8"
PlayerDialog::PlayerDialog(QWidget *parent, bool showChannelList)
    : QDialog(parent)
    , ui(new Ui::PlayerDialog)
{
    ui->setupUi(this);
    m_player=new VideoPlayer;
    connect(m_player, SIGNAL(SIG_getOneImage(QImage)), this, SLOT(slot_setImage(QImage)));
    slot_PlayerStateChanged(PlayerState::Stop);
    //测试
    //->setFileName( _DEF_PATH );
    //connect(&m_timer,SIGNAL(timeout()),this,SLOT());
    connect(m_player,SIGNAL(SIG_PlayerStateChanged(int)),this,SLOT(slot_PlayerStateChanged(int)));
    connect(m_player,SIGNAL(SIG_TotalTime(qint64)),this,SLOT(slot_getTotalTime(qint64)));
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(slot_TimerTimeOut()));
    m_timer.setInterval(500);//超时时间500毫秒
    //安装事件过滤器
    ui->slider_progress->installEventFilter(this);
    
    // 根据参数决定是否显示频道列表
    setChannelListVisible(showChannelList);
    
    // 初始化频道列表
    initChannelList();
    
    // 连接频道列表的双击事件
    connect(ui->listWidget_channels, SIGNAL(itemDoubleClicked(QListWidgetItem*)), 
            this, SLOT(on_listWidget_channels_itemDoubleClicked(QListWidgetItem*)));
}
//获取当前视频时间定时器
void PlayerDialog::slot_TimerTimeOut()
{
    if (QObject::sender() == &m_timer)
    {
        qint64 Sec = m_player->getCurrentTime()/1000000;
        ui->slider_progress->setValue(Sec);
        QString hStr = QString("00%1").arg(Sec/3600);
        QString mStr = QString("00%1").arg(Sec/60%60);
        QString sStr = QString("00%1").arg(Sec%60);
        QString str =
                QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
        ui->lb_curTime->setText(str);
        if(ui->slider_progress->value() == ui->slider_progress->maximum()
                && m_player->playerState() == PlayerState::Stop)
        {
            slot_PlayerStateChanged( PlayerState::Stop );
        }else if(ui->slider_progress->value() + 1 ==
                 ui->slider_progress->maximum()
                 && m_player->playerState() == PlayerState::Stop)
        {
            slot_PlayerStateChanged( PlayerState::Stop );
        }
    }
}
PlayerDialog::~PlayerDialog()
{
    delete ui;
    delete m_player;
}


//qt 线程
//qThread 定义子类 start()->run()

//打开文件播放
#include<QFileDialog>
void PlayerDialog::on_pb_start_clicked()
{
    //开始播放->一段时间内 获取图片
    //m_player->start();
    //打开浏览选择文件
    QString path=QFileDialog::getOpenFileName(this,"打开文件" , "./",
                                              "视频文件 (*.flv *.rmvb *.avi *.MP4 *.mkv);; 所有文件(*.*);;"  );

    //判断
    if(path.isEmpty())return;

    //首先  要先关闭 判断当前的状态stop
    if(m_player->playerState()!=PlayerState::Stop){
        m_player->stop(true);
    }
//----------------------------------------------------------------------------------


    //设置 m_play fileName
    m_player->setFileName(path);
//------------------------------------------------------------------------------
     // m_player->setFileName(_DEF_LIVE_PATH);
    //m_player->setFileName(_DEF_PATH);
    m_player->start();
    slot_PlayerStateChanged(PlayerState::Playing);


    //play
}

void PlayerDialog::slot_setImage(QImage img)
{
    //pixmap image
    //缩放
   //要实现视频加速渲染 OpenGL
    ui->wdg_show->slot_setImage(img);
}




void PlayerDialog::on_pb_resume_clicked()
{
    if(m_player->playerState()!=PlayerState::Pause)return ;
    m_player->play();
    //切换
    ui->pb_resume->hide();
    ui->pb_pause->show();

}


void PlayerDialog::on_pb_pause_clicked()
{
    if(m_player->playerState()!=PlayerState::Playing)return ;
    m_player->pause();
    //切换
    ui->pb_resume->show();
    ui->pb_pause->hide();
}


void PlayerDialog::on_pb_stop_clicked()
{
    m_player->stop(true);
}

void PlayerDialog::slot_PlayerStateChanged(int state)
{
    switch( state )
    {
    case PlayerState::Stop:
        qDebug()<< "VideoPlayer::Stop";
        m_timer.stop();
        ui->slider_progress->setValue(0);
        ui->lb_totalTime->setText("00:00:00");
        ui->lb_curTime->setText("00:00:00");

        ui->pb_pause->hide();
        ui->pb_resume->show();
        this->update();
        isStop = true;
        break;
    case PlayerState::Playing:
        qDebug()<< "VideoPlayer::Playing";
        ui->pb_resume->hide();
        ui->pb_pause->show();
        m_timer.start();
        this->update();
        isStop = false;
        break;
    }
}

void PlayerDialog::slot_getTotalTime(qint64 uSec)
{
    qint64 Sec = uSec/1000000;
    ui->slider_progress->setRange(0,Sec);//精确到秒
    QString hStr = QString("00%1").arg(Sec/3600);
    QString mStr = QString("00%1").arg(Sec/60);
    QString sStr = QString("00%1").arg(Sec%60);
    QString str =
            QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
    ui->lb_totalTime->setText(str);
}
#include<QStyle>
bool PlayerDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->slider_progress) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            int min=ui->slider_progress->minimum();
            int max=ui->slider_progress->maximum();
            int value = QStyle::sliderValueFromPosition(
                        min, max, mouseEvent->pos().x(), ui->slider_progress->width());
            m_timer.stop();
            ui->slider_progress->setValue(value);
            m_player->seek((qint64)value*1000000); //value 秒
            m_timer.start();
            return true;
        } else {
            return false;
        }
    } else {
        //空格 暂停 左右退回/快进 上下 音量调整----------------------------------------------
        // pass the event on to the parent class
        return QDialog::eventFilter(obj, event);
    }
}

// 初始化频道列表
void PlayerDialog::initChannelList()
{
    ui->listWidget_channels->clear();
    QList<ChannelInfo> channels = ChannelManager::instance().getChannels();
    
    for (const ChannelInfo& channel : channels)
    {
        QListWidgetItem* item = new QListWidgetItem(channel.name);
        item->setData(Qt::UserRole, channel.id);  // 存储频道ID
        ui->listWidget_channels->addItem(item);
    }
    
    // 设置列表样式
    ui->listWidget_channels->setAlternatingRowColors(true);
}

// 播放选中的频道
void PlayerDialog::playChannel(const ChannelInfo& channel)
{
    if (channel.urls.isEmpty())
    {
        qDebug() << "频道" << channel.name << "没有可用的URL";
        return;
    }
    
    // 如果当前正在播放，先停止
    if (m_player->playerState() != PlayerState::Stop)
    {
        m_player->stop(true);
    }
    
    // 尝试播放第一个URL
    QString url = channel.urls.first();
    qDebug() << "正在播放频道:" << channel.name << "URL:" << url;
    
    m_player->setFileName(url);
    m_player->start();
    slot_PlayerStateChanged(PlayerState::Playing);
    
    // 更新窗口标题显示当前频道
    this->setWindowTitle(QString("媒体播放器 - %1").arg(channel.name));
}

// 频道列表双击事件
void PlayerDialog::on_listWidget_channels_itemDoubleClicked(QListWidgetItem *item)
{
    int channelId = item->data(Qt::UserRole).toInt();
    ChannelInfo channel = ChannelManager::instance().getChannel(channelId);
    
    if (channel.id > 0)
    {
        playChannel(channel);
    }
}

// 设置频道列表显示/隐藏
void PlayerDialog::setChannelListVisible(bool visible)
{
    if (ui && ui->wdg_channel_list)
    {
        ui->wdg_channel_list->setVisible(visible);
        
        // 如果隐藏频道列表，调整splitter，让视频区域占据全部空间
        if (!visible && ui->splitter)
        {
            // 设置频道列表宽度为0
            ui->splitter->setSizes(QList<int>() << 0 << 1000);
        }
        else if (visible && ui->splitter)
        {
            // 显示频道列表时，设置合适的比例
            ui->splitter->setSizes(QList<int>() << 200 << 800);
        }
    }
}

// 触发打开文件对话框
void PlayerDialog::triggerOpenFile()
{
    on_pb_start_clicked();
}



////跳转
//void VideoPlayer::seek(int64_t pos) //精确到微秒
//{
//    if(!m_videoState.seek_req)
//    {
//        m_videoState.seek_pos = pos;
//        m_videoState.seek_req = 1;
//    }
//}


























































