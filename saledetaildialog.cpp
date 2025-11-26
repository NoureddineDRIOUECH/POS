#include "saledetaildialog.h"
#include "ui_saledetaildialog.h"
#include <QDebug>
#include <QHeaderView>

SaleDetailDialog::SaleDetailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaleDetailDialog)
{
    ui->setupUi(this);
    m_saleItemsModel = new QStandardItemModel(0, 3, this);
    m_saleItemsModel->setHorizontalHeaderLabels({"Product Name", "Quantity Sold", "Price at Sale"});
    ui->saleItemsTableView->setModel(m_saleItemsModel);
    ui->saleItemsTableView->horizontalHeader()->setStretchLastSection(true);
}

SaleDetailDialog::~SaleDetailDialog()
{
    delete ui;
}

void SaleDetailDialog::setSaleId(int saleId, DatabaseManager* dbManager)
{
    m_saleItemsModel->clear();
    m_saleItemsModel->setHorizontalHeaderLabels({"Product Name", "Quantity Sold", "Price at Sale"});

    QList<SaleDetailItem> details = dbManager->getSaleDetails(saleId);
    for (const auto& item : details) {
        QList<QStandardItem*> rowItems;
        rowItems << new QStandardItem(item.productName);
        rowItems << new QStandardItem(QString::number(item.quantitySold));
        rowItems << new QStandardItem(QString::number(item.priceAtSale, 'f', 2));
        m_saleItemsModel->appendRow(rowItems);
    }

    setWindowTitle(QString("Sale Details for Sale ID: %1").arg(saleId));
}
