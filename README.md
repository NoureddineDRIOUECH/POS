# StoreManager - Point of Sale Application

StoreManager is a simple yet functional Point of Sale (POS) application built using Qt 6 and C++. It provides core functionalities for managing product inventory, processing sales, handling user authentication, and generating basic sales reports.

## Features

*   **User Authentication**: Secure login system with a default admin account and password hashing.
*   **Inventory Management**:
    *   Add new products to the inventory.
    *   Edit existing product details.
    *   Delete products from the inventory.
    *   View all products in a table.
    *   Search/filter products by name.
*   **Point of Sale (POS)**:
    *   Browse available products.
    *   Add products to a sales cart.
    *   Adjust product quantities in the cart.
    *   Complete sales transactions, updating inventory and recording sales details.
    *   Cancel ongoing sales.
*   **Sales Reporting**:
    *   View a list of all completed sales.
    *   Double-click on any sale to view its detailed items (products sold, quantity, price at sale).
*   **Persistent Data Storage**: All data is stored locally in an SQLite database.
*   **Modern UI**: Dark theme styling and intuitive layout for a better user experience.

## Database Schema

The application uses an SQLite database with the following tables:

### `Products`
Stores information about each product in the inventory.
```sql
CREATE TABLE Products (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT NOT NULL,
    description TEXT,
    price       REAL NOT NULL,
    quantity    INTEGER NOT NULL
);
```

### `Sales`
Records details of each completed sales transaction.
```sql
CREATE TABLE Sales (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    sale_date     TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    total_amount  REAL NOT NULL
);
```

### `SaleItems`
Links products to specific sales, detailing what was sold in each transaction.
```sql
CREATE TABLE SaleItems (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    sale_id         INTEGER,
    product_id      INTEGER,
    quantity_sold   INTEGER NOT NULL,
    price_at_sale   REAL NOT NULL,
    FOREIGN KEY (sale_id) REFERENCES Sales(id),
    FOREIGN KEY (product_id) REFERENCES Products(id)
);
```

### `Users`
Manages user accounts with hashed passwords for secure authentication.
```sql
CREATE TABLE Users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    role TEXT NOT NULL DEFAULT 'Admin'
);
```

## Prerequisites

*   **Qt 6.x**: The Qt framework (including Qt Widgets and Qt SQL modules).
*   **C++ Compiler**: A C++17 compatible compiler (e.g., GCC, Clang, MSVC).
*   **QMake**: The Qt build system (usually included with Qt installation).

## Building and Running

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-repo/StoreManager.git
    cd StoreManager
    ```
    *(Note: Replace `https://github.com/your-repo/StoreManager.git` with the actual repository URL if this project is hosted.)*

2.  **Open in Qt Creator:**
    *   Launch Qt Creator.
    *   Go to `File > Open File or Project...`.
    *   Navigate to the `StoreManager` directory and select the `POS.pro` file.
    *   Configure the project using your desired Qt kit (usually the latest installed Qt 6 kit).

3.  **Build the Project:**
    *   In Qt Creator, go to `Build > Build Project "POS"`.

4.  **Run the Application:**
    *   In Qt Creator, go to `Build > Run` or click the green "Run" button.

### Manual Build (from terminal)

```bash
qmake POS.pro
make
./POS # Or StoreManager.exe on Windows
```

## Usage

When you run the application:

1.  **Login Screen**:
    *   The application will first present a login dialog.
    *   **Default Admin Credentials**:
        *   **Username**: `admin`
        *   **Password**: `admin`
    *   Enter the credentials and click "Login". If successful, the main application window will appear.

2.  **Main Window - Tabs**:
    The main window has three tabs:

    *   **Inventory**:
        *   Displays all products in a table.
        *   Use the search bar to filter products by name.
        *   **"Add Product"**: Opens a dialog to create a new product entry.
        *   **"Edit Product"**: Select a product from the table and click to open a dialog pre-filled with its details for editing.
        *   **"Delete Product"**: Select a product and click to remove it after a confirmation.

    *   **Point of Sale**:
        *   **Available Products (Left Panel)**: Lists products currently in stock. Click a product to add it to the cart.
        *   **Current Sale Cart (Right Panel)**: Shows items added to the current sale, their quantities, and subtotals.
        *   **Total**: Displays the running total for the current sale.
        *   **"Complete Sale"**: Finalizes the transaction, records the sale, and updates product quantities in inventory.
        *   **"Cancel Sale"**: Clears the current cart without saving the sale.

    *   **Reports**:
        *   Displays a list of all completed sales with their date and total amount.
        *   **Double-click** any sale entry to open a "Sale Details" dialog, showing all individual products that were part of that specific sale.

## Future Enhancements (Potential Ideas)

*   **User Roles**: Implement more granular user roles (e.g., cashier, manager) with different permissions.
*   **Product Quantity Adjustment in Cart**: Add buttons (+/-) to increase/decrease item quantity directly in the sales cart view.
*   **Stock Alerts**: Implement notifications when product quantities fall below a certain threshold.
*   **Advanced Reporting**: Add date range filters, summary reports (e.g., daily/monthly sales), and graphical representations.
*   **Receipt Printing**: Integration with a printer to generate physical receipts.
*   **Error Handling**: More robust error handling and user feedback for database operations.
*   **Product Images**: Add support for product images in the inventory and POS display.
*   **Settings/Configuration**: Allow customization of currency symbols, tax rates, etc.

---

*(This README.md was generated by an AI assistant during the development process.)*
