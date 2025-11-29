#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include "databasemanager.h" // For the User struct

namespace Ui {
class UserDialog;
}

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr);
    ~UserDialog();

    UserData getUserData() const;
    void setUserData(const QString &username, const QString &role);
    bool validateInput();
    void setEditMode(bool isEdit);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::UserDialog *ui;
    bool m_isEditMode;
};

#endif // USERDIALOG_H
