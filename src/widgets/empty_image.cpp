#include "empty_image.h"

#include <QPainter>

#include "ui_empty_image.h"

int constexpr iconSize = 120;
QColor const iconColor = Qt::GlobalColor::gray;

void colorizePixmap(QPixmap& pixmap, QColor const& color)
{
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();
}

EmptyImage::EmptyImage(QWidget* parent)
  : ImageDropWidget(parent)
{
    ui = std::make_unique<Ui::EmptyImage>();
    ui->setupUi(this);

    setAcceptDrops(true);

    QPixmap icon = QIcon(":/icons/icons/image.svg").pixmap(iconSize, iconSize);
    colorizePixmap(icon, iconColor);
    ui->icon->setPixmap(icon);
}

EmptyImage::~EmptyImage() = default;

void EmptyImage::setLabelText(QString const& text)
{
    ui->label->setText(text);
}

void EmptyImage::hideBorder()
{
    ui->frame->setFrameShape(QFrame::NoFrame);
    ui->frame->setStyleSheet("");
}
