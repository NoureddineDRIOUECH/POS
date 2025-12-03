#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <optional> // Use std::optional
#include "databasemanager.h" // For DatabaseManager class and User struct

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
    User getLoggedInUser() const; // Getter for the logged-in user

signals:
    void loginSuccessful();

private slots:
    void on_loginButton_clicked();
    void clearErrorLabel();

private:
    Ui::LoginDialog *ui;
    DatabaseManager* m_dbManager;
    std::optional<User> m_loggedInUser; // Store the logged-in user
};

#endif // LOGINDIALOG_H
