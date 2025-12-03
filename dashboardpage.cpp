#include "dashboardpage.h"
#include "ui_dashboardpage.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QDate> // Add this include for QDate

DashboardPage::DashboardPage(QWidget *parent) : 
    QWidget(parent), 
    ui(new Ui::DashboardPage)
{
    ui->setupUi(this);

    auto applyShadow = [](QWidget *widget) {
        auto *shadow = new QGraphicsDropShadowEffect;
        shadow->setBlurRadius(20);
        shadow->setXOffset(0);
        shadow->setYOffset(0);
        shadow->setColor(QColor(0, 0, 0, 60));
        widget->setGraphicsEffect(shadow);
    };

    applyShadow(ui->cardTotalProducts);
    applyShadow(ui->cardTotalItems);
    applyShadow(ui->cardStockValue);
    applyShadow(ui->cardTotalRevenue);
    applyShadow(ui->cardSalesToday);
    applyShadow(ui->cardSalesMonth);
    applyShadow(ui->weeklySalesCard);
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

    // Set KPI Cards
    ui->totalProductsValueLabel->setText(QString::number(dbManager->getDistinctProductCount()));
    ui->totalItemsValueLabel->setText(QString::number(dbManager->getTotalItemQuantity()));
    ui->stockValueValueLabel->setText(QString::number(dbManager->getTotalStockValue(), 'f', 2) + " MAD");
    ui->totalRevenueValueLabel->setText(QString::number(dbManager->getTotalRevenue(), 'f', 2) + " MAD");
    ui->salesTodayValueLabel->setText(QString::number(dbManager->getSalesCountForToday()));
    ui->salesMonthValueLabel->setText(QString::number(dbManager->getSalesCountForThisMonth()));

    // Sales for Last 7 Days
    QMap<QString, double> salesData = dbManager->getSalesForLast7Days();
    QString salesText;
    
    salesText += "Sales for the last 7 days:\n";
    for (int i = 6; i >= 0; --i) {
        QDate date = QDate::currentDate().addDays(-i);
        QString dayName = date.toString("ddd");
        // The getSalesForLast7Days function already returns a map with 0s for days with no sales.
        double dailySale = salesData.value(dayName, 0.0);
        salesText += QString("%1 (%2): %3 MAD\n").arg(dayName).arg(date.toString("MMM d")).arg(dailySale, 0, 'f', 2);
    }

    ui->weeklySalesTextEdit->setText(salesText);
}
