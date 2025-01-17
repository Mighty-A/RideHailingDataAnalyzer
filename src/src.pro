QT       += core gui network sql qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TARGET = main

TEMPLATE = app

SOURCES += \
    filedialog.cpp \
    fileloadthread.cpp \
    graphdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    # mytilesource.cpp \
    networkmanager.cpp \
    predictiondialog.cpp \
    predictmodel.cpp \
    rangeslider.cpp \
    visualizationdialog.cpp

HEADERS += \
    DataEntry.h \
    filedialog.h \
    fileloadthread.h \
    graphdialog.h \
    mainwindow.h \
    # mytilesource.h \
    networkmanager.h \
    predictiondialog.h \
    predictmodel.h \
    rangeslider.h \
    visualizationdialog.h

FORMS += \
    filedialog.ui \
    graphdialog.ui \
    mainwindow.ui \
    predictiondialog.ui \
    visualizationdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#Linkage for MapGraphics shared library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/MapGraphics/debug/ -lMapGraphics
else:unix:!symbian: LIBS += -L$$OUT_PWD/../MapGraphics/ -lMapGraphics

DISTFILES += \
    ..\README.MD \

INCLUDEPATH += $$PWD/../extern/MapGraphics
DEPENDPATH += $$PWD/../extern/MapGraphics

RESOURCES += \
    resource.qrc

