#ifndef CARTITEM_H
#define CARTITEM_H

#include <QString>

// Simple struct for our in-memory cart
struct CartItem {
    QString name;
    double price;
    int quantity;
};

#endif // CARTITEM_H
