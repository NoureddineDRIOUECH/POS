#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QMap>
#include <QCryptographicHash> // For password hashing
#include "product.h"
#include "cartitem.h"

struct ProductData {
    QString name;
    QString description;
    double price;
    int quantity;
};

struct SaleDetailItem {
    QString productName;
    int quantitySold;
    double priceAtSale;
};

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();
    void init();
    bool addProduct(const ProductData &productData);
    bool deleteProduct(int id);
    bool updateProduct(int id, const ProductData &productData);
    QList<Product> getAllProducts() const;
    Product getProductById(int id) const;
    bool processSale(const QMap<int, CartItem>& cart, double totalAmount);
    void initialSetup();
    bool validateUser(const QString& username, const QString& password) const;
    QSqlDatabase& getDatabase(); // Public getter for m_db
    QList<SaleDetailItem> getSaleDetails(int saleId) const;

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
