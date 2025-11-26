#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "databasemanager.h" // For DatabaseManager class

namespace Ui {
class LoginDialog;
}

class DatabaseManager; // Forward declaration

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    void setDatabaseManager(DatabaseManager* dbManager);

signals:
    void loginSuccessful();

private slots:
    void on_loginButton_clicked();

private:
    Ui::LoginDialog *ui;
    DatabaseManager* m_dbManager;
};

#endif // LOGINDIALOG_H
