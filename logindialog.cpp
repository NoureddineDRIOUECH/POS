#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox> // Will be removed soon, but keep for now
#include <QGraphicsDropShadowEffect>
#include <optional>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    m_dbManager(nullptr) // Initialize pointer to nullptr
{
    ui->setupUi(this);

    // Shadow effect for the central frame
    QGraphicsDropShadowEffect *frameEffect = new QGraphicsDropShadowEffect(this);
    frameEffect->setBlurRadius(25);
    frameEffect->setColor(QColor(0, 0, 0, 80));
    frameEffect->setOffset(5, 5);
    ui->centralFrame->setGraphicsEffect(frameEffect);

    // Shadow effect for the logo
    QGraphicsDropShadowEffect *logoEffect = new QGraphicsDropShadowEffect(this);
    logoEffect->setBlurRadius(15);
    logoEffect->setColor(QColor(0, 0, 0, 100));
    logoEffect->setOffset(2, 2);
    ui->logoLabel->setGraphicsEffect(logoEffect);

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::on_loginButton_clicked);
    
    // Clear error message when user starts typing again
    connect(ui->usernameLineEdit, &QLineEdit::textChanged, this, &LoginDialog::clearErrorLabel);
    connect(ui->passwordLineEdit, &QLineEdit::textChanged, this, &LoginDialog::clearErrorLabel);
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
            // Use the error label instead of a message box
            ui->errorLabel->setText("Invalid username or password.");
        }
    } else {
        // Use the error label for critical errors too
        ui->errorLabel->setText("Error: Database connection failed.");
    }
}

void LoginDialog::clearErrorLabel()
{
    ui->errorLabel->clear();
}
