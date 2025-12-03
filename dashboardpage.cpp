#include "dashboardpage.h"
#include "ui_dashboardpage.h"
#include <QDebug>
#include <QDate> // Add this include for QDate

DashboardPage::DashboardPage(QWidget *parent) : 
    QWidget(parent), 
    ui(new Ui::DashboardPage)
{
    ui->setupUi(this);
}

DashboardPage::~DashboardPage()
{
    delete ui;
}

void DashboardPage::refreshData(DatabaseManager *dbManager)
{
    if (!dbManager) {
        qWarning() << "DatabaseManager is null in DashboardPage::refreshData";
        return;
    }

    // Total Products in Stock
    int totalProducts = dbManager->getTotalProductsInStock();
    ui->totalProductsValue->setText(QString::number(totalProducts));

    // Top Selling Product
    QString topProduct = dbManager->getTopSellingProduct();
    ui->topSellingValue->setText(topProduct);

    // Sales for Last 7 Days
    QMap<QString, double> salesData = dbManager->getSalesForLast7Days();
    QString salesText;
    QList<QString> sortedKeys = salesData.keys();
    // Sort by day of week if you want specific order, otherwise map keys are fine
    // For now, just append in whatever order the map gives them

    // Better to sort by date (assuming keys are "ddd" like "Mon", "Tue" etc, this won't sort chronologically)
    // We should get sales from a function that returns sorted by date, or sort here.
    // Let's iterate through the last 7 days and check the map for data
    salesText += "Sales for the last 7 days:\n";
    for (int i = 6; i >= 0; --i) {
        QDate date = QDate::currentDate().addDays(-i);
        QString dayName = date.toString("ddd");
        double dailySale = salesData.value(dayName, 0.0); // Use value() with default 0.0
        salesText += QString("%1 (%2): $%3\n").arg(dayName).arg(date.toString("MMM d")).arg(dailySale, 0, 'f', 2);
    }

    ui->weeklySalesTextEdit->setText(salesText);
}
