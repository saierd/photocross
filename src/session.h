#pragma once

#include "image.h"

#include <memory>
#include <vector>

class Session : public QObject {
    Q_OBJECT

public:
    void loadImages(QStringList const& filenames);

    bool getWatchFiles() const;
    void setWatchFiles(bool enable);

    std::vector<std::unique_ptr<Image>> const& getImages() const&;

signals:
    void watchFilesChanged(bool enabled) const;
    void imagesChanged() const;

public slots:
    void reload();

private:
    std::vector<std::unique_ptr<Image>> images;

    bool watchFiles = true;
};
