#pragma once

#include <QStringList>

class QPixmap;

class ScreenshotFileManager {
public:
    ScreenshotFileManager();

    QStringList const& getFiles() const&;

    void removeFiles();
    void addImage(QPixmap const& image);

private:
    QString generateFilename() const;

private:
    QStringList files;
};
