#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QMap>
#include <QCryptographicHash> // For password hashing
#include <optional> // Use std::optional instead of QOptional
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

struct User {
    int id;
    QString username;
    QString passwordHash; // Stored hash, not plaintext
    QString role;
};

struct UserData {
    QString username;
    QString password; // Plain text, to be hashed by DatabaseManager
    QString role;
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
    bool processSale(const QMap<int, CartItem>& cart, double totalAmount, int userId);
    void initialSetup();
    void addSampleProducts();
    std::optional<User> validateUser(const QString& username, const QString& password) const;
    QSqlDatabase& getDatabase(); // Public getter for m_db
    QList<SaleDetailItem> getSaleDetails(int saleId) const;

    // User management functions
    bool addUser(const UserData &userData);
    bool updateUser(int id, const UserData &userData);
    bool deleteUser(int id);

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
