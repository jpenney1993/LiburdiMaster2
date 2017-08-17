#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T15:13:43
#
#-------------------------------------------------

QT       += core gui serialbus serialport widgets opengl

LIBS += -lboost_program_options \
        -lv4l2 \
        -ludev \
        -lGLU \
        -lusb-1.0

INCLUDEPATH += Include
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LiburdiMaster
TEMPLATE = app
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        Src/Buffer.cpp \
        Src/CaptureThread.cpp \
        Src/CoreTime.cpp \
        Src/Error.cpp \
        Src/FwUpgradeHelper.cpp \
        Src/Logger.cpp \
        Src/NullSource.cpp \
        Src/QtGlVideoSink.cpp \
        Src/Unity.cpp \
        Src/UsbHelper.cpp \
        Src/Util.cpp \
        Src/VideoBuffer.cpp \
        Src/VideoDevice.cpp \
        Src/VideoDevice_private.cpp \
        Src/VideoDevice_stdctl.cpp \
        Src/VideoDevice_Unity.cpp \
        Src/VideoDevice_xuctl.cpp \
        Src/VideoFrame.cpp \
        Src/Yuyv2Rgb.cpp \
    Settings.cpp \
    liburdiconnectiondialog.cpp \
    ezservoconnectiondialog.cpp

HEADERS += \
        mainwindow.h \
        Include/Buffer.h \
        Include/CaptureThread.h \
        Include/CoreTime.h \
        Include/DeviceDesc.h \
        Include/Error.h \
        Include/ErrorSource.h \
        Include/ErrorSourceBase.h \
        Include/FrameInterval.h \
        Include/FwUpgradeHelper.h \
        Include/guid.h \
        Include/Logger.h \
        Include/NullSource.h \
        Include/PixelFormat.h \
        Include/QtGlVideoSink.h \
        Include/ReferenceCounted.h \
        Include/Unity.h \
        Include/UsbHelper.h \
        Include/Util.h \
        Include/UvcControls.h \
        Include/VideoBuffer.h \
        Include/VideoDevice.h \
        Include/VideoFormat.h \
        Include/VideoFrame.h \
        Include/VideoSink.h \
        Include/VideoSource.h \
        Include/Xu.h \
        Include/Yuyv2Rgb.h \
    Settings.h \
    liburdiconnectiondialog.h \
    ezservoconnectiondialog.h

FORMS += \
        mainwindow.ui \
    liburdiconnectiondialog.ui \
    ezservoconnectiondialog.ui

RESOURCES += \
    resources.qrc
