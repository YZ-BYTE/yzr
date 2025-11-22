#include "uploadvideodialog.h"
#include "ui_uploadvideodialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QImageReader>
#include <QPixmap>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QScreen>
#include <QApplication>
#include <QString>
#include <cstring>
#include <cstdlib>
#include <ctime>

UploadVideoDialog::UploadVideoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UploadVideoDialog)
    , m_tcpClient(nullptr)
    , m_userId(0)
    , m_file(nullptr)
    , m_fileSize(0)
    , m_fileId(0)
    , m_currentBlockIndex(0)
    , m_totalBytesSent(0)
    , m_isUploading(false)
    , m_saveFileThread(nullptr)
    , m_pictureWidget(nullptr)
    , m_isRecording(false)
{
    ui->setupUi(this);
    
    // 设置窗口标题
    setWindowTitle("录制与上传");
    
    // 默认显示上传本地视频标签页
    ui->tabWidget->setCurrentIndex(1);
    
    // 初始化进度条
    ui->progressBar->setValue(0);
    ui->label_progress_percent->setText("0%");
    
    // 初始化上传状态
    resetUploadState();
    
    // 初始化录制功能
    setupRecordThread();
    updateRecordUIState();
}

UploadVideoDialog::~UploadVideoDialog()
{
    if (m_file && m_file->isOpen())
    {
        m_file->close();
        delete m_file;
    }
    
    // 清理录制相关资源
    if (m_saveFileThread)
    {
        if (m_isRecording)
        {
            m_saveFileThread->slot_closeVideo();
        }
        m_saveFileThread->wait();
        delete m_saveFileThread;
        m_saveFileThread = nullptr;
    }
    
    if (m_pictureWidget)
    {
        m_pictureWidget->close();
        delete m_pictureWidget;
        m_pictureWidget = nullptr;
    }
    
    delete ui;
}

void UploadVideoDialog::setTcpClient(INetMediator* tcpClient)
{
    m_tcpClient = tcpClient;
}

void UploadVideoDialog::setUserId(int userId)
{
    m_userId = userId;
}

void UploadVideoDialog::on_pb_browse_clicked()
{
    // 打开文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择视频文件",
        "",
        "视频文件 (*.mp4 *.flv);;所有文件 (*.*)"
    );
    
    if (fileName.isEmpty())
    {
        return;
    }
    
    // 检查文件大小（50MB限制）
    QFileInfo fileInfo(fileName);
    qint64 fileSize = fileInfo.size();
    if (fileSize > 50 * 1024 * 1024) // 50MB
    {
        QMessageBox::warning(this, "提示", "文件大小不能超过50MB");
        return;
    }
    
    // 检查文件格式
    QString suffix = fileInfo.suffix().toLower();
    if (suffix != "mp4" && suffix != "flv")
    {
        QMessageBox::warning(this, "提示", "只支持MP4和FLV格式的文件");
        return;
    }
    
    // 保存文件路径
    m_filePath = fileName;
    ui->le_filePath->setText(fileName);
    
    // 加载缩略图
    loadThumbnail(fileName);
}

void UploadVideoDialog::on_pb_startUpload_clicked()
{
    if (m_isUploading)
    {
        QMessageBox::information(this, "提示", "正在上传中，请稍候...");
        return;
    }
    
    // 检查是否选择了文件
    if (m_filePath.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请先选择要上传的文件");
        return;
    }
    
    // 检查是否选择了至少一个分类
    bool hasCategory = ui->cb_food->isChecked() ||
                       ui->cb_funny->isChecked() ||
                       ui->cb_positive->isChecked() ||
                       ui->cb_dance->isChecked() ||
                       ui->cb_song->isChecked() ||
                       ui->cb_video->isChecked() ||
                       ui->cb_outside->isChecked() ||
                       ui->cb_education->isChecked();
    
    if (!hasCategory)
    {
        QMessageBox::warning(this, "提示", "请至少选择一个视频分类");
        return;
    }
    
    // 检查网络模块
    if (!m_tcpClient)
    {
        QMessageBox::warning(this, "提示", "网络模块未初始化");
        return;
    }
    
    // 检查用户ID
    if (m_userId <= 0)
    {
        QMessageBox::warning(this, "提示", "请先登录");
        return;
    }
    
    // 打开文件
    m_file = new QFile(m_filePath);
    if (!m_file->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "提示", "无法打开文件");
        delete m_file;
        m_file = nullptr;
        return;
    }
    
    // 获取文件信息
    m_fileSize = m_file->size();
    QFileInfo fileInfo(m_filePath);
    QString fileName = fileInfo.fileName();
    
    // 生成文件ID
    m_fileId = generateFileId();
    
    // 重置上传状态
    m_currentBlockIndex = 0;
    m_totalBytesSent = 0;
    m_isUploading = true;
    
    // 禁用上传按钮
    ui->pb_startUpload->setEnabled(false);
    ui->pb_browse->setEnabled(false);
    
    // 发送文件头
    sendFileHeader();
}

void UploadVideoDialog::slot_dealUploadRs(unsigned int lSendIP, char* buf, int nlen)
{
    Q_UNUSED(lSendIP);
    
    if (nlen < (int)sizeof(PackType))
        return;
    
    // 先检查数据包类型
    PackType packType = *(PackType*)buf;
    
    // 只处理上传回复数据包
    if (packType != _DEF_PACK_UPLOAD_RS)
        return;  // 不是上传回复包，忽略
    
    // 验证数据包大小
    if (nlen < (int)sizeof(STRU_UPLOAD_RS))
        return;
    
    STRU_UPLOAD_RS* rs = (STRU_UPLOAD_RS*)buf;
    
    if (rs->m_lResult == upload_success)
    {
        // 服务器确认收到文件头，开始发送文件块
        sendFileBlock();
    }
    else
    {
        QMessageBox::warning(this, "提示", "上传失败");
        resetUploadState();
    }
}

int UploadVideoDialog::generateFileId()
{
    // 使用时间戳和随机数生成文件ID
    static bool seeded = false;
    if (!seeded)
    {
        srand((unsigned int)time(nullptr));
        seeded = true;
    }
    return rand() % 1000000 + 1000;
}

void UploadVideoDialog::collectHobbies(char* hobbies)
{
    // 按照顺序收集分类：美食、搞笑、正能量、舞蹈、歌曲、影视、户外、教育
    hobbies[0] = ui->cb_food->isChecked() ? 1 : 0;
    hobbies[1] = ui->cb_funny->isChecked() ? 1 : 0;
    hobbies[2] = ui->cb_positive->isChecked() ? 1 : 0;
    hobbies[3] = ui->cb_dance->isChecked() ? 1 : 0;
    hobbies[4] = ui->cb_song->isChecked() ? 1 : 0;
    hobbies[5] = ui->cb_video->isChecked() ? 1 : 0;
    hobbies[6] = ui->cb_outside->isChecked() ? 1 : 0;
    hobbies[7] = ui->cb_education->isChecked() ? 1 : 0;
}

void UploadVideoDialog::sendFileHeader()
{
    if (!m_file || !m_file->isOpen())
    {
        return;
    }
    
    QFileInfo fileInfo(m_filePath);
    QString fileName = fileInfo.fileName();
    QString suffix = fileInfo.suffix().toLower();
    
    // 构造上传请求结构体
    STRU_UPLOAD_RQ rq;
    rq.m_UserId = m_userId;
    rq.m_nFileId = m_fileId;
    rq.m_nFileSize = m_fileSize;
    
    // 设置文件类型
    if (suffix == "mp4" || suffix == "flv")
    {
        strcpy_s(rq.m_szFileType, _MAX_SIZE, "video");
    }
    else
    {
        strcpy_s(rq.m_szFileType, _MAX_SIZE, "unknown");
    }
    
    // 设置文件名（兼容中文）
    std::string strName = fileName.toStdString();
    const char* file_name = strName.c_str();
    strcpy_s(rq.m_szFileName, _MAX_PATH, file_name);
    
    // GIF名字（暂时使用文件名）
    strcpy_s(rq.m_szGifName, _MAX_PATH, file_name);
    
    // 收集分类标签
    collectHobbies(rq.m_szHobby);
    
    // 发送文件头
    if (m_tcpClient->SendData(0, (char*)&rq, sizeof(rq)) < 0)
    {
        QMessageBox::warning(this, "提示", "发送文件头失败");
        resetUploadState();
    }
}

void UploadVideoDialog::sendFileBlock()
{
    if (!m_file || !m_file->isOpen())
    {
        return;
    }
    
    // 检查是否已经发送完所有数据
    if (m_totalBytesSent >= m_fileSize)
    {
        // 上传完成
        m_file->close();
        delete m_file;
        m_file = nullptr;
        
        QMessageBox::information(this, "提示", "文件上传完成！");
        resetUploadState();
        return;
    }
    
    // 读取文件块
    char buffer[_DEF_BUFFER];
    qint64 bytesToRead = qMin((qint64)_DEF_BUFFER, m_fileSize - m_totalBytesSent);
    qint64 bytesRead = m_file->read(buffer, bytesToRead);
    
    if (bytesRead <= 0)
    {
        QMessageBox::warning(this, "提示", "读取文件失败");
        resetUploadState();
        return;
    }
    
    // 构造文件块请求结构体
    STRU_FILE_BLOCK_RQ blockRq;
    blockRq.m_nFileId = m_fileId;
    blockRq.m_nBlockIndex = m_currentBlockIndex;
    blockRq.m_nBlockSize = (int)bytesRead;
    memcpy(blockRq.m_szBlockData, buffer, bytesRead);
    
    // 发送文件块
    if (m_tcpClient->SendData(0, (char*)&blockRq, sizeof(blockRq)) < 0)
    {
        QMessageBox::warning(this, "提示", "发送文件块失败");
        resetUploadState();
        return;
    }
    
    // 更新进度
    m_totalBytesSent += bytesRead;
    m_currentBlockIndex++;
    
    // 更新进度条
    updateProgress();
    
    // 继续发送下一个块（使用最小延迟，避免阻塞UI）
    QTimer::singleShot(1, this, &UploadVideoDialog::sendFileBlock);
}

void UploadVideoDialog::updateProgress()
{
    if (m_fileSize > 0)
    {
        int percent = (int)((m_totalBytesSent * 100) / m_fileSize);
        ui->progressBar->setValue(percent);
        ui->label_progress_percent->setText(QString("%1%").arg(percent));
    }
    
    // 如果是录制文件上传，也要更新录制标签页的进度（如果有的话）
    // 这里可以添加额外的进度显示逻辑
}

void UploadVideoDialog::resetUploadState()
{
    m_isUploading = false;
    ui->pb_startUpload->setEnabled(true);
    ui->pb_browse->setEnabled(true);
    ui->progressBar->setValue(0);
    ui->label_progress_percent->setText("0%");
    
    // 如果是从录制文件上传，也要更新上传按钮状态
    if (!m_recordFilePath.isEmpty() && QFile::exists(m_recordFilePath))
    {
        ui->pb_uploadRecorded->setEnabled(true);
    }
    
    if (m_file && m_file->isOpen())
    {
        m_file->close();
        delete m_file;
        m_file = nullptr;
    }
}

void UploadVideoDialog::loadThumbnail(const QString& filePath)
{
    // 默认黑色占位图
    QPixmap pixmap(400, 300);
    pixmap.fill(Qt::black);
    
    // 检查文件是否存在
    if (!QFile::exists(filePath))
    {
        ui->label_thumbnail->setPixmap(pixmap);
        return;
    }
    
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    
    // 如果是图片文件，直接加载
    if (suffix == "jpg" || suffix == "jpeg" || suffix == "png" || suffix == "bmp" || suffix == "gif")
    {
        QImageReader reader(filePath);
        if (reader.canRead())
        {
            QImage image = reader.read();
            if (!image.isNull())
            {
                pixmap = QPixmap::fromImage(image.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                ui->label_thumbnail->setPixmap(pixmap);
                return;
            }
        }
    }
    
    // 如果是视频文件，使用FFmpeg提取第一帧
    if (suffix == "mp4" || suffix == "flv" || suffix == "avi" || suffix == "mkv" || suffix == "mov")
    {
        QImage thumbnail = extractVideoFrame(filePath);
        if (!thumbnail.isNull())
        {
            pixmap = QPixmap::fromImage(thumbnail.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->label_thumbnail->setPixmap(pixmap);
            return;
        }
    }
    
    // 如果所有方法都失败，显示黑色占位图
    ui->label_thumbnail->setPixmap(pixmap);
}

QImage UploadVideoDialog::extractVideoFrame(const QString& filePath)
{
    AVFormatContext *pFormatCtx = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVFrame *pFrame = nullptr;
    AVFrame *pFrameRGB = nullptr;
    AVPacket *packet = nullptr;
    struct SwsContext *img_convert_ctx = nullptr;
    uint8_t *out_buffer_rgb = nullptr;
    int videoStream = -1;
    QImage result;  // 返回的图片，如果失败则为空
    
    // 初始化FFmpeg网络（如果需要）
    avformat_network_init();
    
    // 1. 打开视频文件
    QByteArray ba = filePath.toLocal8Bit();
    const char *filename = ba.constData();
    
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0)
    {
        qDebug() << "Couldn't open video file:" << filePath;
        return result;
    }
    
    // 2. 获取流信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        qDebug() << "Couldn't find stream information";
        avformat_close_input(&pFormatCtx);
        return result;
    }
    
    // 3. 查找视频流
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }
    
    if (videoStream == -1)
    {
        qDebug() << "Didn't find a video stream";
        avformat_close_input(&pFormatCtx);
        return result;
    }
    
    // 4. 获取解码器上下文
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    
    // 5. 查找解码器
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
    {
        qDebug() << "Codec not found";
        avformat_close_input(&pFormatCtx);
        return result;
    }
    
    // 6. 打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        qDebug() << "Could not open codec";
        avformat_close_input(&pFormatCtx);
        return result;
    }
    
    // 7. 分配帧内存
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    if (pFrame == NULL || pFrameRGB == NULL)
    {
        qDebug() << "Could not allocate frame";
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
        if (pFrame) av_frame_free(&pFrame);
        if (pFrameRGB) av_frame_free(&pFrameRGB);
        return result;
    }
    
    // 8. 计算需要的RGB缓冲区大小（使用RGB32格式，与项目其他部分一致）
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);
    out_buffer_rgb = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    
    // 9. 设置RGB帧数据指针
    avpicture_fill((AVPicture *)pFrameRGB, out_buffer_rgb, AV_PIX_FMT_RGB32, 
                   pCodecCtx->width, pCodecCtx->height);
    
    // 10. 初始化图像转换上下文
    img_convert_ctx = sws_getContext(
        pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32,
        SWS_BICUBIC, NULL, NULL, NULL);
    
    if (img_convert_ctx == NULL)
    {
        qDebug() << "Could not initialize conversion context";
        av_free(out_buffer_rgb);
        av_frame_free(&pFrame);
        av_frame_free(&pFrameRGB);
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
        return result;
    }
    
    // 11. 读取第一帧
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    av_init_packet(packet);
    
    int frameFinished = 0;
    bool frameExtracted = false;
    while (av_read_frame(pFormatCtx, packet) >= 0)
    {
        // 只处理视频流
        if (packet->stream_index == videoStream)
        {
            // 解码视频帧
            int ret = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, packet);
            if (ret < 0)
            {
                qDebug() << "Error decoding video frame";
                av_packet_unref(packet);
                break;
            }
            
            // 如果成功解码一帧
            if (frameFinished)
            {
                // 转换图像格式从YUV到RGB
                sws_scale(img_convert_ctx,
                         (uint8_t const * const *)pFrame->data,
                         pFrame->linesize, 0, pCodecCtx->height,
                         pFrameRGB->data, pFrameRGB->linesize);
                
                // 创建QImage（RGB32格式对应Format_RGB32）
                QImage tmpImg((uchar*)out_buffer_rgb, pCodecCtx->width, pCodecCtx->height,
                             pFrameRGB->linesize[0], QImage::Format_RGB32);
                result = tmpImg.copy(); // 深拷贝，避免内存被释放
                
                frameExtracted = true;
                av_packet_unref(packet);
                break; // 只提取第一帧
            }
        }
        av_packet_unref(packet);
    }
    
    // 如果循环结束时packet还没有释放，释放它
    if (packet && !frameExtracted)
    {
        av_packet_unref(packet);
    }
    
    // 12. 清理资源
    if (out_buffer_rgb)
    {
        av_free(out_buffer_rgb);
    }
    if (pFrame)
    {
        av_frame_free(&pFrame);
    }
    if (pFrameRGB)
    {
        av_frame_free(&pFrameRGB);
    }
    if (packet)
    {
        av_free(packet);
    }
    if (img_convert_ctx)
    {
        sws_freeContext(img_convert_ctx);
    }
    if (pCodecCtx)
    {
        avcodec_close(pCodecCtx);
    }
    if (pFormatCtx)
    {
        avformat_close_input(&pFormatCtx);
    }
    
    return result;
}

// 录制相关函数实现
void UploadVideoDialog::setupRecordThread()
{
    // 创建录制线程
    m_saveFileThread = new SaveVideoFileThread();
    
    // 创建画中画预览窗口（隐藏，用于录制）
    m_pictureWidget = new PictureWidget();
    m_pictureWidget->hide();
    m_pictureWidget->move(0, 0);
    
    // 连接录制线程的信号到预览显示
    connect(m_saveFileThread, SIGNAL(SIG_sendVideoFrame(QImage)), 
            this, SLOT(slot_setRecordImage(QImage)));
    connect(m_saveFileThread, SIGNAL(SIG_sendPicInPic(QImage)), 
            m_pictureWidget, SLOT(slot_setImage(QImage)));
}

void UploadVideoDialog::on_pb_startRecord_clicked()
{
    if (m_isRecording)
    {
        QMessageBox::information(this, "提示", "录制正在进行中");
        return;
    }
    
    // 生成录制文件路径
    m_recordFilePath = generateRecordFileName();
    
    // 检查目录是否存在
    QFileInfo fileInfo(m_recordFilePath);
    QDir targetDir = fileInfo.dir();
    if (!targetDir.exists())
    {
        if (!targetDir.mkpath("."))
        {
            QMessageBox::warning(this, "提示", 
                QString("无法创建保存目录：%1").arg(targetDir.absolutePath()));
            return;
        }
    }
    
    // 开始录制
    startRecording();
}

void UploadVideoDialog::on_pb_stopRecord_clicked()
{
    if (!m_isRecording)
    {
        return;
    }
    
    stopRecording();
}

void UploadVideoDialog::on_pb_uploadRecorded_clicked()
{
    // 检查是否选择了至少一个分类
    bool hasCategory = ui->cb_food->isChecked() ||
                       ui->cb_funny->isChecked() ||
                       ui->cb_positive->isChecked() ||
                       ui->cb_dance->isChecked() ||
                       ui->cb_song->isChecked() ||
                       ui->cb_video->isChecked() ||
                       ui->cb_outside->isChecked() ||
                       ui->cb_education->isChecked();
    
    if (!hasCategory)
    {
        QMessageBox::warning(this, "提示", "请至少选择一个视频分类");
        return;
    }
    
    // 检查网络模块
    if (!m_tcpClient)
    {
        QMessageBox::warning(this, "提示", "网络模块未初始化");
        return;
    }
    
    // 检查用户ID
    if (m_userId <= 0)
    {
        QMessageBox::warning(this, "提示", "请先登录");
        return;
    }
    
    // 检查录制文件是否存在
    if (m_recordFilePath.isEmpty() || !QFile::exists(m_recordFilePath))
    {
        QMessageBox::warning(this, "提示", "录制文件不存在");
        return;
    }
    
    // 使用录制的文件路径进行上传
    m_filePath = m_recordFilePath;
    
    // 打开文件
    m_file = new QFile(m_filePath);
    if (!m_file->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "提示", "无法打开录制文件");
        delete m_file;
        m_file = nullptr;
        return;
    }
    
    // 获取文件信息
    m_fileSize = m_file->size();
    
    // 生成文件ID
    m_fileId = generateFileId();
    
    // 重置上传状态
    m_currentBlockIndex = 0;
    m_totalBytesSent = 0;
    m_isUploading = true;
    
    // 禁用上传按钮
    ui->pb_uploadRecorded->setEnabled(false);
    
    // 发送文件头
    sendFileHeader();
}

void UploadVideoDialog::slot_setRecordImage(QImage img)
{
    // 显示录制预览帧
    if (!img.isNull())
    {
        QPixmap pixmap = QPixmap::fromImage(img.scaled(
            ui->label_recordPreview->size(), 
            Qt::KeepAspectRatio, 
            Qt::SmoothTransformation));
        ui->label_recordPreview->setPixmap(pixmap);
    }
}

void UploadVideoDialog::startRecording()
{
    if (!m_saveFileThread)
    {
        QMessageBox::warning(this, "提示", "录制线程未初始化");
        return;
    }
    
    // 设置录制参数
    STRU_AV_FORMAT format;
    format.fileName = m_recordFilePath;
    format.frame_rate = FRAME_RATE;
    format.hasAudio = true;
    format.hasCamera = true;
    format.hasDesk = true;
    format.videoBitRate = 1500000;
    
    // 获取屏幕分辨率
    QScreen *src = QApplication::primaryScreen();
    QRect rect = src->geometry();
    int screenWidth = rect.width();
    int screenHeight = rect.height();
    
    // 限制录制分辨率，避免超过播放器限制（xv 后端最大支持 2048x2048）
    // 使用 1920x1080 作为最大分辨率，保持宽高比
    const int MAX_WIDTH = 1920;
    const int MAX_HEIGHT = 1080;
    
    int recordWidth = screenWidth;
    int recordHeight = screenHeight;
    
    // 如果超过最大分辨率，按比例缩放
    if (recordWidth > MAX_WIDTH || recordHeight > MAX_HEIGHT)
    {
        double widthRatio = (double)MAX_WIDTH / recordWidth;
        double heightRatio = (double)MAX_HEIGHT / recordHeight;
        double ratio = qMin(widthRatio, heightRatio); // 选择较小的比例，确保两个方向都不超过限制
        
        recordWidth = (int)(recordWidth * ratio);
        recordHeight = (int)(recordHeight * ratio);
        
        // 确保宽度和高度都是偶数（视频编码要求）
        recordWidth = (recordWidth / 2) * 2;
        recordHeight = (recordHeight / 2) * 2;
        
        qDebug() << "Recording resolution scaled from" << screenWidth << "x" << screenHeight 
                 << "to" << recordWidth << "x" << recordHeight;
    }
    
    format.width = recordWidth;
    format.height = recordHeight;
    
    // 设置录制信息
    m_saveFileThread->slot_setInfo(format);
    
    // 显示画中画预览窗口（隐藏，用于录制）
    m_pictureWidget->show();
    
    // 开始录制
    m_saveFileThread->slot_openVideo();
    
    // 更新状态
    m_isRecording = true;
    updateRecordUIState();
    
    // 更新文件路径显示
    ui->le_recordPath->setText(m_recordFilePath);
}

void UploadVideoDialog::stopRecording()
{
    if (!m_saveFileThread || !m_isRecording)
    {
        return;
    }
    
    // 隐藏画中画预览窗口
    if (m_pictureWidget)
    {
        m_pictureWidget->hide();
    }
    
    // 停止录制
    m_saveFileThread->slot_closeVideo();
    
    // 等待录制线程完成
    m_saveFileThread->wait();
    
    // 更新状态
    m_isRecording = false;
    updateRecordUIState();
    
    // 验证文件是否存在
    if (QFile::exists(m_recordFilePath))
    {
        QFileInfo fileInfo(m_recordFilePath);
        qint64 fileSize = fileInfo.size();
        if (fileSize > 0)
        {
            QMessageBox::information(this, "提示", 
                QString("录制完成！文件大小：%1 字节").arg(fileSize));
        }
        else
        {
            QMessageBox::warning(this, "提示", "录制文件为空");
        }
    }
    else
    {
        QMessageBox::warning(this, "提示", "录制文件不存在");
    }
}

QString UploadVideoDialog::generateRecordFileName()
{
    // 生成带时间戳的文件名
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString("yyyyMMdd_HHmmss");
    QString fileName = QString("record_%1.flv").arg(timestamp);
    
    // 使用用户目录
    QString dirPath = QDir::homePath() + "/Videos";
    QDir dir;
    if (!dir.exists(dirPath))
    {
        dir.mkpath(dirPath);
    }
    
    return QDir::toNativeSeparators(dirPath + "/" + fileName);
}

void UploadVideoDialog::updateRecordUIState()
{
    if (m_isRecording)
    {
        // 录制中：禁用开始按钮，启用停止按钮
        ui->pb_startRecord->setEnabled(false);
        ui->pb_stopRecord->setEnabled(true);
        ui->pb_uploadRecorded->setEnabled(false);
    }
    else
    {
        // 未录制：启用开始按钮，禁用停止按钮
        ui->pb_startRecord->setEnabled(true);
        ui->pb_stopRecord->setEnabled(false);
        
        // 如果已有录制文件，启用上传按钮
        if (!m_recordFilePath.isEmpty() && QFile::exists(m_recordFilePath))
        {
            ui->pb_uploadRecorded->setEnabled(true);
        }
        else
        {
            ui->pb_uploadRecorded->setEnabled(false);
        }
    }
}

