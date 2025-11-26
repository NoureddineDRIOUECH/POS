#ifndef SALEDETAILDIALOG_H
#define SALEDETAILDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "databasemanager.h" // For SaleDetailItem and DatabaseManager

namespace Ui {
class SaleDetailDialog;
}

class SaleDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaleDetailDialog(QWidget *parent = nullptr);
    ~SaleDetailDialog();

    void setSaleId(int saleId, DatabaseManager* dbManager);

private:
    Ui::SaleDetailDialog *ui;
    QStandardItemModel* m_saleItemsModel;
};

#endif // SALEDETAILDIALOG_H
