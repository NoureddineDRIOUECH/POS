#include "productdialog.h"
#include "ui_productdialog.h"
#include <QFileDialog>
#include <QPixmap>
#include <QDebug>
#include <QMessageBox>

ProductDialog::ProductDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProductDialog)
{
    ui->setupUi(this);
    connect(ui->browseImageButton, &QPushButton::clicked, this, &ProductDialog::on_browseImageButton_clicked);
}

ProductDialog::~ProductDialog()
{
    delete ui;
}

void ProductDialog::setProductData(const QString &name, const QString &desc, double price, int qty, const QString &imagePath)
{
    ui->nameEdit->setText(name);
    ui->descriptionEdit->setPlainText(desc);
    ui->priceSpinBox->setValue(price);
    ui->quantitySpinBox->setValue(qty);
    ui->imagePathEdit->setText(imagePath);
    setImage(imagePath);
}

ProductData ProductDialog::getProductData() const
{
    return {
        ui->nameEdit->text(),
        ui->descriptionEdit->toPlainText(),
        ui->priceSpinBox->value(),
        ui->quantitySpinBox->value(),
        ui->imagePathEdit->text()
    };
}

void ProductDialog::setImage(const QString &imagePath)
{
    if (imagePath.isEmpty()) {
        ui->imagePreviewLabel->setText("No Image");
        ui->imagePreviewLabel->setPixmap(QPixmap());
        return;
    }

    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        qDebug() << "Error loading image:" << imagePath;
        ui->imagePreviewLabel->setText("Failed to load image");
        ui->imagePreviewLabel->setPixmap(QPixmap());
    } else {
        ui->imagePreviewLabel->setPixmap(pixmap.scaled(ui->imagePreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->imagePreviewLabel->setText(""); // Clear "No Image" text
    }
}

void ProductDialog::on_browseImageButton_clicked()
{
    QString selectedImagePath = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif)"));
    if (selectedImagePath.isEmpty()) {
        return;
    }

    QDir imagesDir("images");
    if (!imagesDir.exists()) {
        if (!imagesDir.mkpath(".")) {
            qDebug() << "Error: Could not create images directory.";
            QMessageBox::warning(this, "Error", "Could not create images directory.");
            return;
        }
    }

    QString fileName = QFileInfo(selectedImagePath).fileName();
    QString destinationPath = ":/images/" + fileName; // Use resource path

    // Check if the file is already in resources. If not, copy it.
    // NOTE: For simplicity, we are assuming resource paths are handled by qrc.
    // Real-world applications might copy to a writable location.
    // For this project, we are using the resource system for images.
    // If the selected image is not already in the resources, we should ideally
    // copy it to a temporary writable location and refer to it.
    // However, the current setup uses qrc prefix, so directly copying to a
    // folder outside resources won't work with ":/images/" prefix without recompilation.
    // For this step, we will *simulate* the copy by just updating the path
    // and assuming the image will be added to resources or handled differently later.
    // If the image is to be dynamically loaded from a file system path, the prefix
    // ":/images/" should not be used, and the path should be a full file system path.

    // Given the previous steps, we are using ":/images/" prefix which means
    // the images are expected to be compiled into the application's resources.
    // Dynamic copying to a resource path is not directly possible at runtime.
    // A robust solution would involve copying to a persistent location on disk
    // (e.g., QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/images")
    // and storing that absolute file path in the database.

    // For now, let's assume the user will manage resources or we're just
    // storing the selected file's original path.
    // As per the instruction "Store the selected image file in a designated application folder (e.g., 'images')",
    // and given the ":/images/" prefix used in databasemanager.cpp, the current approach
    // is a bit contradictory.

    // Re-evaluating: If the goal is to save the image *to the local filesystem*
    // and then use that path, then ":/images/" is not suitable for dynamic images.
    // If we want to use ":/images/", then the user must add the image to the .qrc
    // and recompile the application, which is not user-friendly for adding products.

    // Let's modify the approach: Store the *absolute path* to the image file selected by the user.
    // This allows dynamic image selection without requiring recompilation.
    // The `setImage` function already handles loading from an absolute path.
    // The `addSampleProducts` uses ":/images/" as they are built-in resources.
    // For user-selected images, we should store their file system path directly.

    // However, the instruction explicitly states "Store the selected image file in a designated application folder (e.g., 'images')"
    // and "Update the product's image_path in the database to reflect the new file location."
    // This implies copying the file.

    // Let's create a *local copy* of the image in the 'images' folder and use that path.
    // The ":/images/" prefix for *sample products* will remain,
    // but for *user-added products*, we'll use a file system path.

    QString localImagesPath = QDir::currentPath() + "/images/";
    QString destinationFilePath = localImagesPath + fileName;

    if (QFile::copy(selectedImagePath, destinationFilePath)) {
        ui->imagePathEdit->setText(destinationFilePath);
        setImage(destinationFilePath);
        QMessageBox::information(this, "Success", "Image copied successfully to " + destinationFilePath);
    } else {
        if (QFile::exists(destinationFilePath)) {
            // File already exists, just use it
            ui->imagePathEdit->setText(destinationFilePath);
            setImage(destinationFilePath);
            QMessageBox::information(this, "Info", "Image already exists at " + destinationFilePath + ". Using existing file.");
        } else {
            qDebug() << "Error: Could not copy image file from" << selectedImagePath << "to" << destinationFilePath;
            QMessageBox::warning(this, "Error", "Could not copy image file.");
            return;
        }
    }
}
