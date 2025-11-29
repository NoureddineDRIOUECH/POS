#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QCryptographicHash>
#include <optional>

DatabaseManager::DatabaseManager()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("store.db");

    if (!m_db.open()) {
        qDebug() << "Error: connection with database failed:" << m_db.lastError();
    } else {
        qDebug() << "Database: connection ok";
    }
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void DatabaseManager::init()
{
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return;
    }

    QSqlQuery query;

    // Create Products table
    if (!query.exec("CREATE TABLE IF NOT EXISTS Products ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "description TEXT, "
                    "price REAL NOT NULL, "
                    "quantity INTEGER NOT NULL, "
                    "image_path TEXT"
                    ");")) {
        qDebug() << "Error: failed to create Products table:" << query.lastError();
    } else {
        // Add image_path column if it doesn't exist
        if (!query.exec("PRAGMA table_info(Products);")) {
            qDebug() << "Error: failed to get table info for Products:" << query.lastError();
        } else {
            bool columnExists = false;
            while (query.next()) {
                if (query.value("name").toString() == "image_path") {
                    columnExists = true;
                    break;
                }
            }
            if (!columnExists) {
                if (!query.exec("ALTER TABLE Products ADD COLUMN image_path TEXT;")) {
                    qDebug() << "Error: failed to add image_path column to Products table:" << query.lastError();
                } else {
                    qDebug() << "Added image_path column to Products table.";
                }
            }
        }
    }

    // Create Sales table
    if (!query.exec("CREATE TABLE IF NOT EXISTS Sales ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "sale_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                    "total_amount REAL NOT NULL, "
                    "user_id INTEGER REFERENCES Users(id)"
                    ");")) {
        qDebug() << "Error: failed to create Sales table:" << query.lastError();
    }

    // Create SaleItems table
    if (!query.exec("CREATE TABLE IF NOT EXISTS SaleItems ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "sale_id INTEGER, "
                    "product_id INTEGER, "
                    "quantity_sold INTEGER NOT NULL, "
                    "price_at_sale REAL NOT NULL, "
                    "FOREIGN KEY (sale_id) REFERENCES Sales(id), "
                    "FOREIGN KEY (product_id) REFERENCES Products(id)"
                    ");")) {
        qDebug() << "Error: failed to create SaleItems table:" << query.lastError();
    }

    // Create Users table
    if (!query.exec("CREATE TABLE IF NOT EXISTS Users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT NOT NULL UNIQUE, "
                    "password_hash TEXT NOT NULL, "
                    "role TEXT NOT NULL DEFAULT 'Admin'"
                    ");")) {
        qDebug() << "Error: failed to create Users table:" << query.lastError();
    }
}

bool DatabaseManager::addProduct(const ProductData &productData)
{
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Products (name, description, price, quantity, image_path) "
                  "VALUES (:name, :description, :price, :quantity, :image_path)");
    query.bindValue(":name", productData.name);
    query.bindValue(":description", productData.description);
    query.bindValue(":price", productData.price);
    query.bindValue(":quantity", productData.quantity);
    query.bindValue(":image_path", productData.imagePath);

    if (!query.exec()) {
        qDebug() << "Error: failed to add product:" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteProduct(int id)
{
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM Products WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error: failed to delete product:" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseManager::updateProduct(int id, const ProductData &productData)
{
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE Products SET name = :name, description = :description, "
                  "price = :price, quantity = :quantity, image_path = :image_path WHERE id = :id");
    query.bindValue(":name", productData.name);
    query.bindValue(":description", productData.description);
    query.bindValue(":price", productData.price);
    query.bindValue(":quantity", productData.quantity);
    query.bindValue(":image_path", productData.imagePath);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error: failed to update product:" << query.lastError();
        return false;
    }

    return true;
}

QList<Product> DatabaseManager::getAllProducts() const
{
    QList<Product> products;
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return products;
    }
    QSqlQuery query("SELECT id, name, price, quantity, image_path FROM Products");
    while (query.next()) {
        products.append({
            query.value("id").toInt(),
            query.value("name").toString(),
            query.value("price").toDouble(),
            query.value("quantity").toInt(),
            query.value("image_path").toString()
        });
    }
    return products;
}

Product DatabaseManager::getProductById(int id) const
{
    Product product;
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return product;
    }

    QSqlQuery query;
    query.prepare("SELECT id, name, price, quantity, image_path FROM Products WHERE id = :id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        product.id = query.value("id").toInt();
        product.name = query.value("name").toString();
        product.price = query.value("price").toDouble();
        product.quantity = query.value("quantity").toInt();
        product.imagePath = query.value("image_path").toString();
    } else {
        qDebug() << "Error: failed to get product by id:" << query.lastError();
    }
    return product;
}

bool DatabaseManager::processSale(const QMap<int, CartItem>& cart, double totalAmount, int userId) {
    // Transactions ensure that all operations succeed or none do.
    if (!m_db.transaction()) {
        qDebug() << "Failed to start transaction:" << m_db.lastError();
        return false;
    }

    // 1. Insert into Sales table
    QSqlQuery saleQuery;
    saleQuery.prepare("INSERT INTO Sales (total_amount, user_id) VALUES (:total, :user_id)");
    saleQuery.bindValue(":total", totalAmount);
    saleQuery.bindValue(":user_id", userId);
    if (!saleQuery.exec()) {
        qDebug() << "Sale insert failed:" << saleQuery.lastError();
        m_db.rollback();
        return false;
    }
    int saleId = saleQuery.lastInsertId().toInt();

    // 2. Insert each cart item into SaleItems and update Products stock
    for (auto it = cart.constBegin(); it != cart.constEnd(); ++it) {
        int productId = it.key();
        const CartItem& item = it.value();

        // Insert into SaleItems
        QSqlQuery itemQuery;
        itemQuery.prepare("INSERT INTO SaleItems (sale_id, product_id, quantity_sold, price_at_sale) "
                          "VALUES (:sale_id, :product_id, :qty, :price)");
        itemQuery.bindValue(":sale_id", saleId);
        itemQuery.bindValue(":product_id", productId);
        itemQuery.bindValue(":qty", item.quantity);
        itemQuery.bindValue(":price", item.price);
        if (!itemQuery.exec()) {
            qDebug() << "SaleItems insert failed:" << itemQuery.lastError();
            m_db.rollback();
            return false;
        }

        // Update product quantity
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE Products SET quantity = quantity - :qty WHERE id = :id");
        updateQuery.bindValue(":qty", item.quantity);
        updateQuery.bindValue(":id", productId);
        if (!updateQuery.exec()) {
            qDebug() << "Product quantity update failed:" << updateQuery.lastError();
            m_db.rollback();
            return false;
        }
    }

    // If all operations were successful, commit the transaction
    return m_db.commit();
}

void DatabaseManager::initialSetup() {
    // This method should be called once after creating tables.
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM Users");
    if (query.next() && query.value(0).toInt() == 0) {
        qDebug() << "No users found. Creating default admin user.";
        
        QString username = "admin";
        // IMPORTANT: NEVER store plain text passwords!
        QString password = "admin"; 
        QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);

        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO Users (username, password_hash, role) VALUES (:user, :pass, 'Admin')");
        insertQuery.bindValue(":user", username);
        insertQuery.bindValue(":pass", passwordHash.toHex());
        if (!insertQuery.exec()) {
            qDebug() << "Failed to create default admin:" << insertQuery.lastError();
        }
    }
}

std::optional<User> DatabaseManager::validateUser(const QString& username, const QString& password) const {
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);

    QSqlQuery query;
    query.prepare("SELECT id, username, password_hash, role FROM Users WHERE username = :user");
    query.bindValue(":user", username);
    query.exec();

    if (query.next()) {
        if (query.value("password_hash").toByteArray() == passwordHash.toHex()) {
            User user;
            user.id = query.value("id").toInt();
            user.username = query.value("username").toString();
            user.passwordHash = query.value("password_hash").toByteArray(); // Store the hash
            user.role = query.value("role").toString();
            return std::optional<User>(user);
        }
    }
    return std::nullopt; // User not found or password incorrect
}

QSqlDatabase& DatabaseManager::getDatabase()
{
    return m_db;
}

QList<SaleDetailItem> DatabaseManager::getSaleDetails(int saleId) const
{
    QList<SaleDetailItem> details;
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return details;
    }

    QSqlQuery query;
    query.prepare("SELECT P.name, SI.quantity_sold, SI.price_at_sale, P.image_path "
                  "FROM SaleItems SI JOIN Products P ON SI.product_id = P.id "
                  "WHERE SI.sale_id = :sale_id");
    query.bindValue(":sale_id", saleId);

    if (!query.exec()) {
        qDebug() << "Error: failed to get sale details:" << query.lastError();
        return details;
    }

    while (query.next()) {
        details.append({
            query.value("name").toString(),
            query.value("quantity_sold").toInt(),
            query.value("price_at_sale").toDouble(),
            query.value("image_path").toString()
        });
    }
    return details;
}

bool DatabaseManager::addUser(const UserData &userData)
{
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return false;
    }

    QByteArray passwordHash = QCryptographicHash::hash(userData.password.toUtf8(), QCryptographicHash::Sha256);

    QSqlQuery query;
    query.prepare("INSERT INTO Users (username, password_hash, role) "
                  "VALUES (:username, :password_hash, :role)");
    query.bindValue(":username", userData.username);
    query.bindValue(":password_hash", passwordHash.toHex());
    query.bindValue(":role", userData.role);

    if (!query.exec()) {
        qDebug() << "Error: failed to add user:" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseManager::updateUser(int id, const UserData &userData)
{
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return false;
    }

    QSqlQuery query;
    bool passwordChanged = !userData.password.isEmpty();

    QString queryString = "UPDATE Users SET username = :username, role = :role";
    if (passwordChanged) {
        queryString += ", password_hash = :password_hash";
    }
    queryString += " WHERE id = :id";

    query.prepare(queryString);

    query.bindValue(":username", userData.username);
    query.bindValue(":role", userData.role);
    if (passwordChanged) {
        QByteArray passwordHash = QCryptographicHash::hash(userData.password.toUtf8(), QCryptographicHash::Sha256);
        query.bindValue(":password_hash", passwordHash.toHex());
    }
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error: failed to update user:" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteUser(int id)
{
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM Users WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error: failed to delete user:" << query.lastError();
        return false;
    }

    return true;
}

void DatabaseManager::addSampleProducts()
{
    if (!m_db.isOpen()) {
        qDebug() << "Error: database is not open";
        return;
    }

    QSqlQuery countQuery;
    countQuery.exec("SELECT COUNT(*) FROM Products");
    if (countQuery.next() && countQuery.value(0).toInt() == 0) {
        qDebug() << "No products found. Creating sample products.";

        addProduct({ "Laptop", "A powerful laptop", 1200.00, 10, ":/images/laptop.png" });
        addProduct({ "Mouse", "A wireless mouse", 25.00, 50, ":/images/mouse.png" });
        addProduct({ "Keyboard", "A mechanical keyboard", 75.00, 30, ":/images/keyboard.png" });
    }
}
