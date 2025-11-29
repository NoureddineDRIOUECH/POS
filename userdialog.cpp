#include "userdialog.h"
#include "ui_userdialog.h"
#include <QMessageBox>

UserDialog::UserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDialog),
    m_isEditMode(false) // Default to add mode
{
    ui->setupUi(this);

    // Disconnect the default accepted signal and connect to our custom slot
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &UserDialog::on_buttonBox_accepted);
}

UserDialog::~UserDialog()
{
    delete ui;
}

UserData UserDialog::getUserData() const
{
    UserData data;
    data.username = ui->usernameLineEdit->text();
    data.password = ui->passwordLineEdit->text(); // Password will be empty if not changed during edit
    data.role = ui->roleComboBox->currentText();
    return data;
}

void UserDialog::setUserData(const QString &username, const QString &role)
{
    ui->usernameLineEdit->setText(username);
    ui->roleComboBox->setCurrentText(role);
    // Password fields are left blank for security
}

void UserDialog::setEditMode(bool isEdit)
{
    m_isEditMode = isEdit;
}

bool UserDialog::validateInput()
{
    if (ui->usernameLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username cannot be empty.");
        return false;
    }

    if (!m_isEditMode && ui->passwordLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Password cannot be empty for a new user.");
        return false;
    }

    if (ui->passwordLineEdit->text() != ui->confirmPasswordLineEdit->text()) {
        QMessageBox::warning(this, "Password Mismatch", "The passwords entered do not match.");
        return false;
    }
    return true;
}

void UserDialog::on_buttonBox_accepted()
{
    if (validateInput()) {
        accept();
    }
}
