#-------------------------------------------------
#
# Project created by QtCreator 2018-09-12T14:29:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = videoImageDisplay
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    global.cpp

HEADERS  += mainwindow.h \
    global.h

FORMS    += mainwindow.ui

#add for opencv start by Henry
INCLUDEPATH+=/home/work/common/opencv/install/include \
             /home/work/common/opencv/install/include/opencv \
             /home/work/common/opencv/install/include/opencv2

LIBS += /home/work/common/opencv/install/lib/libopencv_highgui.so \
        /home/work/common/opencv/install/lib/libopencv_core.so    \
        /home/work/common/opencv/install/lib/libopencv_imgproc.so   \
        /home/work/common/opencv/install/lib/libopencv_imgcodecs.so \
        /home/work/common/opencv/install/lib/libopencv_videoio.so
#add for opencv end
