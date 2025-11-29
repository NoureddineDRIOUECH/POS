#include "mainwindow.h"
#include "logindialog.h" // Include the new dialog
#include "databasemanager.h" // Include the db manager
#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

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
    // Pass the dbManager to the dialog for validation
    loginDialog.setDatabaseManager(&dbManager); 
    
    MainWindow w;
    w.setDatabaseManager(&dbManager); // Pass the db manager to the main window
    
    // Show the login dialog. The program will pause here.
    if (loginDialog.exec() == QDialog::Accepted) {
        // If login was successful, setup the main window and show it.
        w.postLoginSetup(loginDialog.getLoggedInUser());
        w.show();
        return a.exec();
    } else {
        // If the user closes the login dialog, exit the application.
        return 0;
    }
}
