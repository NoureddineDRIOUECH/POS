#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QMessageBox> // Added for potential error messages
#include <QMap>
#include <QListWidgetItem>
#include "product.h"
#include "cartitem.h"

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

private slots:
    void on_addProductButton_clicked();
    void on_editProductButton_clicked();
    void on_deleteProductButton_clicked();
    void onProductListItemClicked(QListWidgetItem *item);
    void updateCartView();
    void onCompleteSaleClicked();
    void onCancelSaleClicked();
    void on_searchLineEdit_textChanged(const QString &text);

private:
    Ui::MainWindow *ui;
    QSqlTableModel *m_productsModel; // Declare the model
    QSqlTableModel *m_salesModel; // Declare the sales model
    DatabaseManager *m_dbManager;
    QMap<int, CartItem> m_cart; // Key: product_id, Value: CartItem
    QStandardItemModel *m_cartModel;

    void setupPosTab();
};
#endif // MAINWINDOW_H
