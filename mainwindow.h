#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QMessageBox> // Added for potential error messages
#include <QMap>
#include <QListWidgetItem>
#include "product.h"
#include "cartitem.h"
#include "databasemanager.h" // For User struct

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class DatabaseManager; // Forward declaration
class QStandardItemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setDatabaseManager(DatabaseManager *dbManager);
    void postLoginSetup(const User &user);

private slots:
    void on_addProductButton_clicked();
    void on_editProductButton_clicked();
    void on_deleteProductButton_clicked();
    void onProductListViewClicked(const QModelIndex &index);
    void updateCartView();
    void onCompleteSaleClicked();
    void onCancelSaleClicked();
    void on_searchLineEdit_textChanged(const QString &text);
    void on_salesTableView_doubleClicked(const QModelIndex &index);

    // User Management Slots
    void on_addUserButton_clicked();
    void on_editUserButton_clicked();
    void on_deleteUserButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *m_productsModel; // Declare the model
    QSqlTableModel *m_salesModel; // Declare the sales model
    QSqlTableModel *m_usersModel; // Declare the users model
    QSortFilterProxyModel *m_proxyModel;
    DatabaseManager *m_dbManager;
    QMap<int, CartItem> m_cart; // Key: product_id, Value: CartItem
    QStandardItemModel *m_posProductsModel;
    QStandardItemModel *m_cartModel;
    User m_currentUser; // Store the currently logged-in user

    void setupPosTab();
    void applyPermissions();
};
#endif // MAINWINDOW_H
