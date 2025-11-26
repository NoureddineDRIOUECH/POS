#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>
#include "databasemanager.h" // For ProductData struct

namespace Ui {
class ProductDialog;
}

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductDialog(QWidget *parent = nullptr);
    ~ProductDialog();

    void setProductData(const QString &name, const QString &desc, double price, int qty);
    ProductData getProductData() const;

private:
    Ui::ProductDialog *ui;
};

#endif // PRODUCTDIALOG_H
