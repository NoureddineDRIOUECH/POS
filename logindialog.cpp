#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox> // For displaying warning messages
#include <optional>

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

User LoginDialog::getLoggedInUser() const
{
    return m_loggedInUser.value(); // Assumes this is called only if a user is logged in
}

void LoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (m_dbManager) {
        std::optional<User> authenticatedUser = m_dbManager->validateUser(username, password);
        if (authenticatedUser.has_value()) {
            m_loggedInUser = authenticatedUser; // Store the authenticated user
            accept(); // Close dialog with success
        } else {
            QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
        }
    } else {
        QMessageBox::critical(this, "Error", "Database manager not set.");
    }
}
