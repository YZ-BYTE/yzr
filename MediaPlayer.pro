QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += SDL_MAIN_HANDLED

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(./netapi/netapi.pri)
INCLUDEPATH +=./netapi/

SOURCES += \
    ckernal.cpp \
    PacketQueue.cpp \
    main.cpp \
    playerdialog.cpp \
    videoplayer.cpp \
    channelinfo.cpp \
    mainwindow.cpp \
    logindialog.cpp \
    uploadvideodialog.cpp \
    md5util.cpp \
    recorder/audio_read.cpp \
    recorder/picinpic_read.cpp \
    recorder/picturewidget.cpp \
    recorder/recorderdialog.cpp \
    recorder/savevideofilethread.cpp

HEADERS += \
    ckernal.h \
    PacketQueue.h \
    playerdialog.h \
    videoplayer.h \
    channelinfo.h \
    mainwindow.h \
    logindialog.h \
    uploadvideodialog.h \
    md5util.h \
    recorder/audio_read.h \
    recorder/common.h \
    recorder/picinpic_read.h \
    recorder/picturewidget.h \
    recorder/recorderdialog.h \
    recorder/savevideofilethread.h

FORMS += \
    playerdialog.ui \
    mainwindow.ui \
    logindialog.ui \
    uploadvideodialog.ui \
    recorder/picturewidget.ui \
    recorder/recorderdialog.ui

include(./opengl/opengl.pri)
INCLUDEPATH += ./opengl/

INCLUDEPATH += $$PWD/ffmpeg-4.2.2/include\
                $$PWD/SDL2-2.0.10/include\
                $$PWD/recorder

INCLUDEPATH += D:\\ColinVideoProject\\opencv\\opencv-release/include/opencv2\
               D:\\ColinVideoProject\\opencv\\opencv-release/include

LIBS += $$PWD/ffmpeg-4.2.2/lib/avcodec.lib\
        $$PWD/ffmpeg-4.2.2/lib/avdevice.lib\
        $$PWD/ffmpeg-4.2.2/lib/avfilter.lib\
        $$PWD/ffmpeg-4.2.2/lib/avformat.lib\
        $$PWD/ffmpeg-4.2.2/lib/avutil.lib\
        $$PWD/ffmpeg-4.2.2/lib/postproc.lib\
        $$PWD/ffmpeg-4.2.2/lib/swresample.lib\
        $$PWD/ffmpeg-4.2.2/lib/swscale.lib\
        $$PWD/SDL2-2.0.10/lib/x86/SDL2.lib

LIBS += D:\\ColinVideoProject\\opencv\\opencv-release\\lib\\libopencv_*.dll.a

# 复制依赖 dll 到输出目录，确保运行时可加载
win32:QMAKE_POST_LINK += $$quote(cmd /c xcopy /y /d \"$$PWD\\dll\\*.dll\" \"$$OUT_PWD\\\" >nul)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
