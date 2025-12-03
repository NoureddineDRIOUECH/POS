#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasemanager.h"
#include "productdialog.h" // Include the dialog header
#include "saledetaildialog.h" // Include the sale detail dialog header
#include "userdialog.h" // Include UserDialog
#include <QDate>
#include <QDebug> // Include QDebug for debugging purposes
#include <QModelIndex>
#include <QStandardItemModel>
#include <QStyle> // For standard icons
#include <QMessageBox>

// Remove 'using namespace QtCharts;'
// All QtCharts classes will be explicitly qualified with 'QtCharts::'

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Instantiate DashboardPage and add it to the stacked widget
    m_dashboardPage = new DashboardPage(this);
    ui->contentStackedWidget->addWidget(m_dashboardPage); // Add as the first page (index 0)

    // DatabaseManager is now set from main.cpp, so we don't create it here.
    // The pointer m_dbManager will be null until set.
    m_dbManager = nullptr;
    m_posProductsModel = nullptr;
    m_proxyModel = nullptr;
}

void MainWindow::setDatabaseManager(DatabaseManager *dbManager)
{
    m_dbManager = dbManager;

    // Now that we have the db manager, we can set up the models
    // Initialize and configure the QSqlTableModel for products
    m_productsModel = new QSqlTableModel(this);
    m_productsModel->setTable("Products");
    m_productsModel->select(); // Populate the model with data

    // Set user-friendly header names for products
    m_productsModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_productsModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    m_productsModel->setHeaderData(2, Qt::Horizontal, tr("Description"));
    m_productsModel->setHeaderData(3, Qt::Horizontal, tr("Price"));
    m_productsModel->setHeaderData(4, Qt::Horizontal, tr("Quantity"));
    m_productsModel->setHeaderData(5, Qt::Horizontal, tr("Image Path"));

    // Link the product model to the QTableView
    ui->productsTableView->setModel(m_productsModel);
    ui->productsTableView->hideColumn(0); // Hide ID
    ui->productsTableView->hideColumn(5); // Hide Image Path
    
    // Initialize and configure the QSqlTableModel for sales (reports tab)
    m_salesModel = new QSqlTableModel(this, m_dbManager->getDatabase()); 
    m_salesModel->setTable("Sales");
    m_salesModel->select();
    m_salesModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    m_salesModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Date"));
    m_salesModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Total Amount"));

    ui->salesTableView->setModel(m_salesModel);
    ui->salesTableView->hideColumn(0); // Hide ID
    ui->salesTableView->resizeColumnsToContents();

    // Initialize and configure the QSqlTableModel for users (user management tab)
    m_usersModel = new QSqlTableModel(this, m_dbManager->getDatabase());
    m_usersModel->setTable("Users");
    m_usersModel->select();
    m_usersModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    m_usersModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Username"));
    m_usersModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Password Hash")); // Will be hidden
    m_usersModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Role"));

    ui->usersTableView->setModel(m_usersModel);
    ui->usersTableView->hideColumn(0); // Hide ID
    ui->usersTableView->hideColumn(2); // Hide password hash
    ui->usersTableView->resizeColumnsToContents();

    // Initialize the cart model
    m_cartModel = new QStandardItemModel(0, 3, this);
    m_cartModel->setHorizontalHeaderLabels({"Product", "Quantity", "Subtotal"});
    ui->cartTableView->setModel(m_cartModel);

    // Initialize the proxy model for filtering
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setFilterRole(Qt::DisplayRole); // Filter based on the display role (text)

    // Call setupPosTab to populate m_posProductsModel
    setupPosTab();

    m_proxyModel->setSourceModel(m_posProductsModel);
    ui->posProductListView->setModel(m_proxyModel);
    
    // Configure the POS product list for a grid view
    ui->posProductListView->setViewMode(QListView::IconMode);
    ui->posProductListView->setIconSize(QSize(100, 100));
    ui->posProductListView->setGridSize(QSize(130, 130));
    ui->posProductListView->setResizeMode(QListView::Adjust);
    ui->posProductListView->setMovement(QListView::Static);
    ui->posProductListView->setWordWrap(true);

    // Set icons for buttons
    ui->addProductButton->setIcon(QIcon(":/images/plus-circle.svg"));
    ui->editProductButton->setIcon(QIcon(":/images/edit.svg"));
    ui->deleteProductButton->setIcon(QIcon(":/images/trash-2.svg"));
    ui->completeSaleButton->setIcon(QIcon(":/images/check-circle.svg"));
    ui->cancelSaleButton->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton)); // Keep this default for now
    
    // Connect signals and slots
    connect(ui->posProductListView, &QListView::clicked, this, &MainWindow::onProductListViewClicked);
    connect(ui->completeSaleButton, &QPushButton::clicked, this, &MainWindow::onCompleteSaleClicked);
    connect(ui->cancelSaleButton, &QPushButton::clicked, this, &MainWindow::onCancelSaleClicked);
    connect(ui->navigationListWidget, &QListWidget::currentRowChanged, this, &MainWindow::on_navigationListWidget_currentRowChanged);
}

void MainWindow::postLoginSetup(const User &user)
{
    // Scale logo to fit the label without distortion
    QPixmap logoPixmap(":/images/poslogo.png");
    if (!logoPixmap.isNull()) {
        ui->logoLabel->setPixmap(logoPixmap.scaled(ui->logoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    m_currentUser = user;
    ui->greetingLabel->setText(QString("Hello, %1").arg(user.username));
    qDebug() << "User" << m_currentUser.username << "logged in with role" << m_currentUser.role;

    // Apply permissions and setup tabs now that the user is logged in
    applyPermissions();
    setupPosTab();
    updateStatsBar();
}

MainWindow::~MainWindow()
{
    delete m_productsModel; // Clean up the product model
    delete m_salesModel;   // Clean up the sales model
    delete m_usersModel; // Clean up the users model
    delete m_posProductsModel; // Clean up the pos products model
    delete m_proxyModel; // Clean up the proxy model
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
            setupPosTab(); // Refresh the POS tab
            updateStatsBar();
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
    QString imagePath = m_productsModel->data(m_productsModel->index(selectedIndex.row(), 5)).toString(); // Retrieve image_path

    ProductDialog dialog(this);
    dialog.setProductData(name, description, price, quantity, imagePath);

    if (dialog.exec() == QDialog::Accepted) {
        ProductData data = dialog.getProductData();
        if (m_dbManager->updateProduct(id, data)) {
            m_productsModel->select(); // Refresh the model
            setupPosTab(); // Refresh the POS tab
            updateStatsBar();
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
            setupPosTab(); // Refresh the POS tab
            updateStatsBar();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete product from the database.");
        }
    }
}

void MainWindow::on_searchLineEdit_textChanged(const QString &text)
{
    if (m_proxyModel) {
        m_proxyModel->setFilterFixedString(text);
        m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    }
}

void MainWindow::setupPosTab()
{
    if (!m_posProductsModel) {
        m_posProductsModel = new QStandardItemModel(this);
    }
    m_posProductsModel->clear();
    ui->posProductListView->setModel(m_posProductsModel);

    QList<Product> products = m_dbManager->getAllProducts();
    for (const auto& product : qAsConst(products)) {
        if (product.quantity > 0) { // Only show items that are in stock
            // Format text with a newline for better layout in grid view
            QString itemText = QString("%1\n$%2").arg(product.name).arg(product.price, 0, 'f', 2);
            QStandardItem* item = new QStandardItem(itemText);
            
            item->setData(product.id, Qt::UserRole); // Store the ID invisibly with the item
            item->setTextAlignment(Qt::AlignBottom | Qt::AlignHCenter);

            // Load and set the icon
            if (!product.imagePath.isEmpty()) {
                QPixmap pixmap(product.imagePath);
                if (!pixmap.isNull()) {
                    item->setIcon(QIcon(pixmap));
                } else {
                    qDebug() << "Error loading image for product" << product.name << ":" << product.imagePath;
                    // Optional: set a placeholder icon here
                }
            }
            m_posProductsModel->appendRow(item);
        }
    }
}

void MainWindow::onProductListViewClicked(const QModelIndex &index)
{
    int productId = index.data(Qt::UserRole).toInt();

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
    for(const auto& item : qAsConst(m_cart)) total += item.price * item.quantity;

    bool success = m_dbManager->processSale(m_cart, total, m_currentUser.id);

    if (success) {
        QMessageBox::information(this, "Success", "Sale completed successfully!");
        onCancelSaleClicked(); // Clear the cart
        m_productsModel->select(); // Refresh inventory view
        m_salesModel->select();    // Refresh sales view
        setupPosTab(); // Refresh POS product list (to update quantities)
        updateStatsBar();
    } else {
        QMessageBox::critical(this, "Error", "Failed to process the sale. Check database connection.");
    }
}

void MainWindow::onCancelSaleClicked()
{
    m_cart.clear();
    updateCartView(); // This will clear the table and reset the total
}

void MainWindow::on_salesTableView_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int saleId = m_salesModel->data(m_salesModel->index(index.row(), 0)).toInt(); // Column 0 is ID
    
    SaleDetailDialog dialog(this);
    dialog.setSaleId(saleId, m_dbManager);
    dialog.exec();
}

// User Management Slots

void MainWindow::on_addUserButton_clicked()
{
    UserDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        UserData data = dialog.getUserData();
        
        // Basic validation
        if (data.username.isEmpty() || data.password.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Username and password cannot be empty.");
            return;
        }

        if (m_dbManager->addUser(data)) {
            m_usersModel->select(); // Refresh the model
        } else {
            QMessageBox::warning(this, "Database Error", "Failed to add user. The username might already exist.");
        }
    }
}

void MainWindow::on_editUserButton_clicked()
{
    QModelIndexList selectedRows = ui->usersTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::information(this, "Select User", "Please select a user to edit.");
        return;
    }

    QModelIndex selectedIndex = selectedRows.first();
    int id = m_usersModel->data(m_usersModel->index(selectedIndex.row(), 0)).toInt();
    QString username = m_usersModel->data(m_usersModel->index(selectedIndex.row(), 1)).toString();
    QString role = m_usersModel->data(m_usersModel->index(selectedIndex.row(), 3)).toString();

    UserDialog dialog(this);
    dialog.setEditMode(true); // Set dialog to edit mode
    dialog.setUserData(username, role);

    if (dialog.exec() == QDialog::Accepted) {
        UserData data = dialog.getUserData();
        
        // Basic validation
        if (data.username.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Username cannot be empty.");
            return;
        }

        if (m_dbManager->updateUser(id, data)) {
            m_usersModel->select(); // Refresh the model
        } else {
            QMessageBox::warning(this, "Database Error", "Failed to update user.");
        }
    }
}

void MainWindow::on_deleteUserButton_clicked()
{
    QModelIndexList selectedRows = ui->usersTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::information(this, "Select User", "Please select a user to delete.");
        return;
    }

    QModelIndex selectedIndex = selectedRows.first();
    int id = m_usersModel->data(m_usersModel->index(selectedIndex.row(), 0)).toInt();
    QString username = m_usersModel->data(m_usersModel->index(selectedIndex.row(), 1)).toString();

    // Prevent deleting the currently logged-in user
    if (id == m_currentUser.id) {
        QMessageBox::warning(this, "Action Forbidden", "You cannot delete the currently logged-in user.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete User", "Are you sure you want to delete user '" + username + "'?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_dbManager->deleteUser(id)) {
            m_usersModel->select(); // Refresh the model
        } else {
            QMessageBox::warning(this, "Database Error", "Failed to delete user from the database.");
        }
    }
}

void MainWindow::applyPermissions()
{
    // This function now just triggers the navigation setup,
    // which internally handles the role-based logic.
    setupNavigation();
}

void MainWindow::setupNavigation()
{
    ui->navigationListWidget->clear();

    const QSize iconSize = ui->navigationListWidget->iconSize();

    // Add Dashboard item (Index 0)
    QPixmap dashboardPixmap(":/images/dash.png");
    QIcon dashboardIcon(dashboardPixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QListWidgetItem *dashboardItem = new QListWidgetItem(dashboardIcon, "Dashboard");
    ui->navigationListWidget->addItem(dashboardItem);

    // Add Point of Sale item (Index 1)
    QPixmap posPixmap(":/images/poos.png");
    QIcon posIcon(posPixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QListWidgetItem *posItem = new QListWidgetItem(posIcon, "Point of Sale");
    ui->navigationListWidget->addItem(posItem);

    // Add Admin-only items (Indices 2, 3, 4)
    if (m_currentUser.role == "Admin") {
        // Inventory Icon (Index 2)
        QPixmap docPixmap(":/images/document.png");
        QIcon docIcon(docPixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        QListWidgetItem *inventoryItem = new QListWidgetItem(docIcon, "Inventory");
        ui->navigationListWidget->addItem(inventoryItem);

        // Reports Icon (Index 3)
        QPixmap stockPixmap(":/images/en-stock.png");
        QIcon stockIcon(stockPixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        QListWidgetItem *reportsItem = new QListWidgetItem(stockIcon, "Reports");
        ui->navigationListWidget->addItem(reportsItem);

        // User Management Icon (Index 4)
        QPixmap profilPixmap(":/images/profil.png");
        QIcon profilIcon(profilPixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        QListWidgetItem *usersItem = new QListWidgetItem(profilIcon, "User Management");
        ui->navigationListWidget->addItem(usersItem);
    }
    
    // Set the default selected item to be the first one (Dashboard)
    if (ui->navigationListWidget->count() > 0) {
        ui->navigationListWidget->setCurrentRow(0);
    }
}

void MainWindow::on_navigationListWidget_currentRowChanged(int row)
{
    if (row < 0) return;

    QListWidgetItem* item = ui->navigationListWidget->item(row);
    if (!item) return;

    const QString text = item->text();

    if (text == "Dashboard") {
        // Refresh dashboard data when dashboard page is selected
        if (m_dashboardPage && m_dbManager) {
            m_dashboardPage->refreshData(m_dbManager);
        }
        ui->contentStackedWidget->setCurrentWidget(m_dashboardPage);
    } else if (text == "Point of Sale") {
        ui->contentStackedWidget->setCurrentWidget(ui->posPage);
    } else if (text == "Inventory") {
        ui->contentStackedWidget->setCurrentWidget(ui->inventoryPage);
    } else if (text == "Reports") {
        ui->contentStackedWidget->setCurrentWidget(ui->reportsPage);
    } else if (text == "User Management") {
        ui->contentStackedWidget->setCurrentWidget(ui->usersPage);
    }
}

// Helper function to format large numbers
static QString formatValue(double value) {
    if (value >= 1000000) {
        return QString("$%1M").arg(value / 1000000.0, 0, 'f', 1);
    } else if (value >= 1000) {
        return QString("$%1K").arg(value / 1000.0, 0, 'f', 1);
    }
    return QString("$%1").arg(value, 0, 'f', 2);
}

void MainWindow::updateStatsBar()
{
    if (!m_dbManager) return;

    double revenue = m_dbManager->getTotalRevenue();
    double stockValue = m_dbManager->getTotalStockValue();

    ui->revenueValueLabel->setText(formatValue(revenue));
    ui->stockValueLabel->setText(formatValue(stockValue));
}


