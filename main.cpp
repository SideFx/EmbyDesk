/////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Purpose:     Main, load Go library, apply style
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-05-06
/////////////////////////////////////////////////////////////////////////////

#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "jbgolib.hpp"
#include "apicall.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    GoLib api;
    if (!api.load()) return -1; // LoadLibrary("jbembyapi")
#if defined (Q_OS_WIN)
    a.setStyle(QStyleFactory::create("fusion"));
#elif defined (Q_OS_MAC)
    a.setStyle(QStyleFactory::create("macos"));
#endif
    qRegisterMetaType<EmbyCollection>("embyCollection");
    APICall::setAPI(&api);
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
