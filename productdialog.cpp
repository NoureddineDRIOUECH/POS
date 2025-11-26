#include "productdialog.h"
#include "ui_productdialog.h"

ProductDialog::ProductDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProductDialog)
{
    ui->setupUi(this);
}

ProductDialog::~ProductDialog()
{
    delete ui;
}

void ProductDialog::setProductData(const QString &name, const QString &desc, double price, int qty)
{
    ui->nameEdit->setText(name);
    ui->descriptionEdit->setPlainText(desc);
    ui->priceSpinBox->setValue(price);
    ui->quantitySpinBox->setValue(qty);
}

ProductData ProductDialog::getProductData() const
{
    return {
        ui->nameEdit->text(),
        ui->descriptionEdit->toPlainText(),
        ui->priceSpinBox->value(),
        ui->quantitySpinBox->value()
    };
}
