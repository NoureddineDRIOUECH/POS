#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

struct Product {
    int id;
    QString name;
    double price;
    int quantity;
    QString imagePath;
};

#endif // PRODUCT_H
