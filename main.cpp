#include "mainwindow.h"
#include "logindialog.h" // Include the new dialog
#include "databasemanager.h" // Include the db manager
#include <QApplication>
#include <QIcon>
#include <QFile>
#include <QTextStream>
#include <QFontDatabase>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/iconapp.png"));

    // Load custom fonts from resources
    QDir fontDir(":/fonts/Font/");
    if (fontDir.exists()) {
        for (const QString &fontFile : fontDir.entryList(QStringList() << "*.ttf", QDir::Files)) {
            int fontId = QFontDatabase::addApplicationFont(fontDir.filePath(fontFile));
            if (fontId == -1) {
                qWarning() << "Failed to load font:" << fontDir.filePath(fontFile);
            }
        }
    } else {
        qWarning() << "Font resource directory not found: :/fonts/Font/";
    }

    QFile styleFile(":/style.qss");
    if (!styleFile.open(QFile::ReadOnly)) {
        qWarning("Failed to open stylesheet file.");
    }
    QString styleSheet = QLatin1String(styleFile.readAll());
    a.setStyleSheet(styleSheet);

    // Create the one and only DatabaseManager instance
    DatabaseManager dbManager; // Instantiate the manager
    dbManager.init(); // Initialize tables
    dbManager.initialSetup(); // Create default admin if needed
    dbManager.addSampleProducts(); // Create sample products if needed

    LoginDialog loginDialog;
    loginDialog.setDatabaseManager(&dbManager);

    MainWindow w;
    w.setDatabaseManager(&dbManager);

    QObject::connect(&w, &MainWindow::userLoggedOut, &loginDialog, [&]() {
        w.hide();
        if (loginDialog.exec() == QDialog::Accepted) {
            w.postLoginSetup(loginDialog.getLoggedInUser());
            w.show();
        } else {
            QApplication::quit();
        }
    });

    if (loginDialog.exec() == QDialog::Accepted) {
        w.postLoginSetup(loginDialog.getLoggedInUser());
        w.show();
        return a.exec();
    }

    return 0;
}
