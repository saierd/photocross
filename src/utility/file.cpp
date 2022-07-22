#include "file.h"

#include <QFileDialog>
#include <QMimeDatabase>

namespace {

char const* imageFileFilter = "Image Files (*.jpg *.jpeg *.png *.bmp)";
char const* defaultImageFileSuffix = "png";

}  // namespace

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

QString selectImageFileForSaving(QString const& defaultFileName, QWidget* parent)
{
    QFileDialog dialog(parent, "Save Image...", defaultFileName, imageFileFilter);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.selectNameFilter(imageFileFilter);
    dialog.setDefaultSuffix(defaultImageFileSuffix);
    if (dialog.exec() != 0) {
        auto selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.empty()) {
            return selectedFiles[0];
        }
    }

    return {};
}