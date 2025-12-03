#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>
#include "databasemanager.h" // For DatabaseManager class

namespace Ui {
class DashboardPage;
}

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);
    ~DashboardPage();

public slots:
    void refreshData(DatabaseManager *dbManager);

private:
    Ui::DashboardPage *ui;
};

#endif // DASHBOARDPAGE_H
