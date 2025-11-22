#ifndef UPLOADVIDEODIALOG_H
#define UPLOADVIDEODIALOG_H

#include <QDialog>
#include <QFile>
#include <QTimer>
#include <QImage>
#include <QDateTime>
#include "netapi/mediator/INetMediator.h"
#include "netapi/net/packdef.h"
#include "recorder/savevideofilethread.h"
#include "recorder/picturewidget.h"
#include "recorder/common.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UploadVideoDialog; }
QT_END_NAMESPACE

class UploadVideoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UploadVideoDialog(QWidget *parent = nullptr);
    ~UploadVideoDialog();

    // 设置网络模块
    void setTcpClient(INetMediator* tcpClient);
    
    // 设置用户ID
    void setUserId(int userId);

public slots:
    // 处理服务器响应
    void slot_dealUploadRs(unsigned int lSendIP, char* buf, int nlen);

private slots:
    // 浏览文件按钮
    void on_pb_browse_clicked();
    
    // 开始上传按钮
    void on_pb_startUpload_clicked();
    
    // 录制相关槽函数
    void on_pb_startRecord_clicked();
    void on_pb_stopRecord_clicked();
    void on_pb_uploadRecorded_clicked();
    void slot_setRecordImage(QImage img);

private:
    Ui::UploadVideoDialog *ui;
    INetMediator* m_tcpClient;
    int m_userId;
    
    // 文件相关
    QString m_filePath;
    QFile* m_file;
    qint64 m_fileSize;
    int m_fileId;
    int m_currentBlockIndex;
    qint64 m_totalBytesSent;
    
    // 上传状态
    bool m_isUploading;
    
    // 录制相关
    SaveVideoFileThread* m_saveFileThread;
    PictureWidget* m_pictureWidget;
    QString m_recordFilePath;
    bool m_isRecording;
    
    // 生成文件ID（使用随机数）
    int generateFileId();
    
    // 收集选中的分类标签
    void collectHobbies(char* hobbies);
    
    // 发送文件头
    void sendFileHeader();
    
    // 发送文件块
    void sendFileBlock();
    
    // 更新进度条
    void updateProgress();
    
    // 重置上传状态
    void resetUploadState();
    
    // 获取视频缩略图（使用FFmpeg提取视频第一帧）
    void loadThumbnail(const QString& filePath);
    
    // 从视频文件中提取第一帧作为缩略图
    QImage extractVideoFrame(const QString& filePath);
    
    // 录制相关辅助函数
    void setupRecordThread();
    void startRecording();
    void stopRecording();
    QString generateRecordFileName();
    void updateRecordUIState();
};

#endif // UPLOADVIDEODIALOG_H

