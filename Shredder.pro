#-------------------------------------------------
#
# Project created by QtCreator 2012-12-20T15:17:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Shredder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\\opencv\\release\\install\\include\

LIBS += -LC:\\opencv\\release\\install\\bin \
    -lopencv_core240 \
    -lopencv_highgui240 \
    -lopencv_imgproc240 \
    -lopencv_features2d240 \
    -lopencv_calib3d240 \
    -lopencv_objdetect240 \

OTHER_FILES += \
    haarcascade_frontalface_alt.xml


target.path += $${OUT_PWD}
target.files = haarcascade_frontalface_alt.xml

INSTALLS += target
