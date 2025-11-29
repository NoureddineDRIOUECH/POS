QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    databasemanager.cpp \
    productdialog.cpp \
    logindialog.cpp \
    saledetaildialog.cpp \
    userdialog.cpp

HEADERS += \
    mainwindow.h \
    databasemanager.h \
    productdialog.h \
    product.h \
    cartitem.h \
    logindialog.h \
    saledetaildialog.h \
    userdialog.h

FORMS += \
    mainwindow.ui \
    productdialog.ui \
    logindialog.ui \
    saledetaildialog.ui \
    userdialog.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target