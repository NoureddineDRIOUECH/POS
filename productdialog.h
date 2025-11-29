#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>
#include "databasemanager.h" // For ProductData struct
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


namespace Ui {
class ProductDialog;
}

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductDialog(QWidget *parent = nullptr);
    ~ProductDialog();

    void setProductData(const QString &name, const QString &desc, double price, int qty, const QString &imagePath);
    ProductData getProductData() const;
    void setImage(const QString &imagePath);

private slots:
    void on_browseImageButton_clicked();

private:
    Ui::ProductDialog *ui;
};

#endif // PRODUCTDIALOG_H
