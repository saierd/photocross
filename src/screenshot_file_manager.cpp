#include "screenshot_file_manager.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QTemporaryFile>

QString temporaryFilePrefix()
{
    return QApplication::applicationName() + "_";
}

ScreenshotFileManager::ScreenshotFileManager()
{
    QString const nameFilter = temporaryFilePrefix() + "*.png";
    QDir::Filters filter = QDir::Files | QDir::Readable | QDir::CaseSensitive;

    auto const entries = QDir::temp().entryInfoList({nameFilter}, filter);
    for (auto const& entry : entries) {
        files.push_back(entry.absoluteFilePath());
    }
}

QStringList const& ScreenshotFileManager::getFiles() const
{
    return files;
}

void ScreenshotFileManager::removeFiles()
{
    for (auto const& file : qAsConst(files)) {
        QFile::remove(file);
    }
    files.clear();
}

void ScreenshotFileManager::addImage(QPixmap const& image)
{
    QString file = generateFilename();
    image.save(file);
    files.push_back(file);
}

QString ScreenshotFileManager::generateFilename() const
{
    QString index = QString::number(files.size()).rightJustified(3, '0');
    QString pattern = temporaryFilePrefix() + index + "_XXXXXX.png";

    QTemporaryFile file(QDir::tempPath() + "/" + pattern);
    file.setAutoRemove(false);
    file.open();
    return file.fileName();
}
