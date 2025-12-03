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
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif)"));
    if (imagePath.isEmpty()) {
        return;
    }

    QDir imagesDir("images");
    if (!imagesDir.exists()) {
        imagesDir.mkpath(".");
    }

    QString fileName = QFileInfo(imagePath).fileName();
    QString destinationPath = imagesDir.filePath(fileName);

    if (!QFile::exists(destinationPath)) {
        if (!QFile::copy(imagePath, destinationPath)) {
            QMessageBox::warning(this, tr("Error"), tr("Could not copy image to application folder."));
            return;
        }
    }

    // Store the relative path to be portable
    ui->imagePathEdit->setText(destinationPath);
    setImage(destinationPath);
}
