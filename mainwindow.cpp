#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasemanager.h"
#include "productdialog.h" // Include the dialog header
#include <QDebug> // Include QDebug for debugging purposes
#include <QModelIndex>
#include <QStandardItemModel>
#include <QStyle> // For standard icons

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize DatabaseManager and create tables
    m_dbManager = new DatabaseManager(); // Use the member variable
    m_dbManager->init();

    // Initialize and configure the QSqlTableModel
    m_productsModel = new QSqlTableModel(this);
    m_productsModel->setTable("Products");
    m_productsModel->select(); // Populate the model with data

    // Set user-friendly header names
    m_productsModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_productsModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    m_productsModel->setHeaderData(2, Qt::Horizontal, tr("Description"));
    m_productsModel->setHeaderData(3, Qt::Horizontal, tr("Price"));
    m_productsModel->setHeaderData(4, Qt::Horizontal, tr("Quantity"));

    // Link the model to the QTableView
    ui->productsTableView->setModel(m_productsModel);

    // Hide the ID column
    ui->productsTableView->hideColumn(0); // ID column is at index 0
    
    // Initialize the cart model
    m_cartModel = new QStandardItemModel(0, 3, this);
    m_cartModel->setHorizontalHeaderLabels({"Product", "Quantity", "Subtotal"});
    ui->cartTableView->setModel(m_cartModel);

    setupPosTab();
    
    // Set icons for buttons
    ui->addProductButton->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    ui->editProductButton->setIcon(style()->standardIcon(QStyle::SP_DialogYesButton));
    ui->deleteProductButton->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    ui->completeSaleButton->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
    ui->cancelSaleButton->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    
    // Connect the list widget's click signal
    connect(ui->posProductListWidget, &QListWidget::itemClicked, this, &MainWindow::onProductListItemClicked);
    
    // Connect the sale buttons
    connect(ui->completeSaleButton, &QPushButton::clicked, this, &MainWindow::onCompleteSaleClicked);
    connect(ui->cancelSaleButton, &QPushButton::clicked, this, &MainWindow::onCancelSaleClicked);

    // Connect the search line edit
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::on_searchLineEdit_textChanged);
}

MainWindow::~MainWindow()
{
    delete m_productsModel; // Clean up the model
    delete m_dbManager;   // Clean up the database manager
    // m_cartModel is parented to 'this', so it's deleted automatically
    delete ui;
}

void MainWindow::on_addProductButton_clicked()
{
    ProductDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ProductData data = dialog.getProductData();
        if (m_dbManager->addProduct(data)) {
            m_productsModel->select(); // Refresh the model
        } else {
            QMessageBox::warning(this, "Error", "Failed to add product to the database.");
        }
    }
}

void MainWindow::on_editProductButton_clicked()
{
    QModelIndexList selectedRows = ui->productsTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::information(this, "Select Product", "Please select a product to edit.");
        return;
    }

    QModelIndex selectedIndex = selectedRows.first();
    int id = m_productsModel->data(m_productsModel->index(selectedIndex.row(), 0)).toInt();
    QString name = m_productsModel->data(m_productsModel->index(selectedIndex.row(), 1)).toString();
    QString description = m_productsModel->data(m_productsModel->index(selectedIndex.row(), 2)).toString();
    double price = m_productsModel->data(m_productsModel->index(selectedIndex.row(), 3)).toDouble();
    int quantity = m_productsModel->data(m_productsModel->index(selectedIndex.row(), 4)).toInt();

    ProductDialog dialog(this);
    dialog.setProductData(name, description, price, quantity);

    if (dialog.exec() == QDialog::Accepted) {
        ProductData data = dialog.getProductData();
        if (m_dbManager->updateProduct(id, data)) {
            m_productsModel->select(); // Refresh the model
        } else {
            QMessageBox::warning(this, "Error", "Failed to update product in the database.");
        }
    }
}

void MainWindow::on_deleteProductButton_clicked()
{
    QModelIndexList selectedRows = ui->productsTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::information(this, "Select Product", "Please select a product to delete.");
        return;
    }

    QModelIndex selectedIndex = selectedRows.first();
    int id = m_productsModel->data(m_productsModel->index(selectedIndex.row(), 0)).toInt();
    QString name = m_productsModel->data(m_productsModel->index(selectedIndex.row(), 1)).toString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Product", "Are you sure you want to delete '" + name + "'?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_dbManager->deleteProduct(id)) {
            m_productsModel->select(); // Refresh the model
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete product from the database.");
        }
    }
}

void MainWindow::on_searchLineEdit_textChanged(const QString &text)
{
    // Filter the QSqlTableModel based on the product name (column 1)
    m_productsModel->setFilter(QString("name LIKE '%%1%'").arg(text));
    m_productsModel->select(); // Apply the filter
}

void MainWindow::setupPosTab()
{
    ui->posProductListWidget->clear();
    QList<Product> products = m_dbManager->getAllProducts();
    for (const auto& product : products) {
        if (product.quantity > 0) { // Only show items that are in stock
            QListWidgetItem* item = new QListWidgetItem(QString("%1 - $%2").arg(product.name).arg(product.price));
            item->setData(Qt::UserRole, product.id); // Store the ID invisibly with the item
            ui->posProductListWidget->addItem(item);
        }
    }
}

void MainWindow::onProductListItemClicked(QListWidgetItem *item)
{
    int productId = item->data(Qt::UserRole).toInt();

    // Get product details from the database to ensure they are current
    Product p = m_dbManager->getProductById(productId);
    
    if (m_cart.contains(productId)) {
        // If item is already in cart, just increase quantity
        m_cart[productId].quantity++;
    } else {
        // Otherwise, add new item to cart
        m_cart[productId] = { p.name, p.price, 1 };
    }
    
    updateCartView();
}

void MainWindow::updateCartView()
{
    m_cartModel->removeRows(0, m_cartModel->rowCount());

    double total = 0.0;
    for (auto it = m_cart.constBegin(); it != m_cart.constEnd(); ++it) {
        const CartItem& item = it.value();
        QList<QStandardItem*> rowItems;
        rowItems << new QStandardItem(item.name);
        rowItems << new QStandardItem(QString::number(item.quantity));
        rowItems << new QStandardItem(QString::number(item.price * item.quantity, 'f', 2));
        m_cartModel->appendRow(rowItems);
        total += item.price * item.quantity;
    }
    
    ui->totalAmountLabel->setText(QString("Total: $%1").arg(total, 0, 'f', 2));
}

void MainWindow::onCompleteSaleClicked()
{
    if (m_cart.isEmpty()) {
        QMessageBox::warning(this, "Empty Cart", "Cannot complete an empty sale.");
        return;
    }

    double total = 0.0;
    for(const auto& item : m_cart) total += item.price * item.quantity;

    bool success = m_dbManager->processSale(m_cart, total);

    if (success) {
        QMessageBox::information(this, "Success", "Sale completed successfully!");
        onCancelSaleClicked(); // Clear the cart
        m_productsModel->select(); // Refresh inventory view
        setupPosTab(); // Refresh POS product list (to update quantities)
    } else {
        QMessageBox::critical(this, "Error", "Failed to process the sale. Check database connection.");
    }
}

void MainWindow::onCancelSaleClicked()
{
    m_cart.clear();
    updateCartView(); // This will clear the table and reset the total
}
