#include "saledetaildialog.h"
#include "ui_saledetaildialog.h"
#include <QDebug>
#include <QHeaderView>
#include <QPixmap>

SaleDetailDialog::SaleDetailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaleDetailDialog)
{
    ui->setupUi(this);
    m_saleItemsModel = new QStandardItemModel(0, 4, this); // 4 columns now: Name, Qty, Price, Image
    m_saleItemsModel->setHorizontalHeaderLabels({"Product Name", "Quantity Sold", "Price at Sale", "Image"});
    ui->saleItemsTableView->setModel(m_saleItemsModel);
    ui->saleItemsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->saleItemsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Make table read-only
    ui->saleItemsTableView->verticalHeader()->setDefaultSectionSize(60); // Adjust row height for images
    ui->saleItemsTableView->setColumnWidth(3, 80); // Adjust image column width
}

SaleDetailDialog::~SaleDetailDialog()
{
    delete ui;
}

void SaleDetailDialog::setSaleId(int saleId, DatabaseManager* dbManager)
{
    m_saleItemsModel->clear();
    m_saleItemsModel->setHorizontalHeaderLabels({"Product Name", "Quantity Sold", "Price at Sale", "Image"});

    QList<SaleDetailItem> details = dbManager->getSaleDetails(saleId);
    for (const auto& item : details) {
        QList<QStandardItem*> rowItems;
        rowItems << new QStandardItem(item.productName);
        rowItems << new QStandardItem(QString::number(item.quantitySold));
        rowItems << new QStandardItem(QString::number(item.priceAtSale, 'f', 2));

        // Handle image display
        QStandardItem* imageItem = new QStandardItem();
        if (!item.imagePath.isEmpty()) {
            QPixmap pixmap(item.imagePath);
            if (!pixmap.isNull()) {
                QPixmap scaledPixmap = pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                imageItem->setIcon(QIcon(scaledPixmap));
            } else {
                qDebug() << "Error loading image for sale item" << item.productName << ":" << item.imagePath;
                imageItem->setText("No Image");
            }
        } else {
            imageItem->setText("No Image");
        }
        rowItems << imageItem;
        m_saleItemsModel->appendRow(rowItems);
    }

    setWindowTitle(QString("Sale Details for Sale ID: %1").arg(saleId));
}
