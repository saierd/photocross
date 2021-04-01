#include "file_helpers.h"

#include <QFileDialog>
#include <QMimeDatabase>

char const* imageFileFilter = "Image (*.jpg *.jpeg *.png *.bmp)";

bool isImageFile(QString const& file)
{
    return QMimeDatabase().mimeTypeForFile(file).name().startsWith("image/");
}

QString selectImageFile(QWidget* parent)
{
    return QFileDialog::getOpenFileName(parent, "Open Image...", {}, imageFileFilter);
}

QStringList selectImageFiles(QWidget* parent)
{
    return QFileDialog::getOpenFileNames(parent, "Open Images...", {}, imageFileFilter);
}
