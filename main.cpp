#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Load stylesheet
    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
        file.close();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
