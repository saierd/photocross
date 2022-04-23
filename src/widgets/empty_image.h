#pragma once

#include "widgets/image_drop_widget.h"

#include <memory>

namespace Ui {
class EmptyImage;
}

class EmptyImage : public ImageDropWidget {
    Q_OBJECT

public:
    explicit EmptyImage(QWidget* parent = nullptr);
    ~EmptyImage() override;

    void setLabelText(QString const& text);
    void hideBorder();

private:
    std::unique_ptr<Ui::EmptyImage> ui;
};
