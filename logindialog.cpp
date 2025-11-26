#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox> // For displaying warning messages

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    m_dbManager(nullptr) // Initialize pointer to nullptr
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::on_loginButton_clicked);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setDatabaseManager(DatabaseManager* dbManager)
{
    m_dbManager = dbManager;
}

void LoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (m_dbManager && m_dbManager->validateUser(username, password)) {
        accept(); // Close dialog with success
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}
