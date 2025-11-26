#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QMap>
#include "product.h"
#include "cartitem.h"

struct ProductData {
    QString name;
    QString description;
    double price;
    int quantity;
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

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
