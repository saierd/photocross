#pragma once

#include "widgets/image_view_scene.h"

#include <QWidget>

#include <memory>
#include <optional>
#include <vector>

namespace Ui {
class SessionView;
}

class ImageView;
class MouseIndicatorGraphicsItem;
class SourceImageView;
class Session;

class SessionView : public QWidget {
    Q_OBJECT

public:
    explicit SessionView(QWidget* parent = nullptr);
    ~SessionView() override;

    void setSession(Session* session);

    bool getSourceImagesVisible() const;
    void setSourceImagesVisible(bool enable);

    bool getMouseIndicatorsVisible() const;
    void setMouseIndicatorsVisible(bool enable);

    bool getAutoFitInView() const;
    void setAutoFitInView(bool enable);

    bool getLayoutIsHorizontal() const;

    bool canSaveComparisonView() const;

signals:
    void sourceImagesVisibleChanged(bool sourceImagesVisible);
    void mouseIndicatorsVisibleChanged(bool mouseIndicatorsVisible);
    void autoFitInViewChanged(bool autoFitInView);
    void layoutDirectionChanged(bool layoutIsHorizontal);

    void canSaveComparisonViewChanged(bool enabled);

public slots:
    void flipLayoutDirection();

    void zoomIn();
    void zoomOut();
    void fitToView();
    void fitComparisonImageToView();

    void rotateImagesLeft();
    void rotateImagesRight();

    void saveComparisonView();

private:
    using MouseIndicatorPosition = ImageViewScene::MouseMovedInfo;

private slots:
    void adjustNumberOfImageViews(size_t numImages);
    void updateImages();
    void updateComparisonView();
    void imageViewReachedZoomOutLimit();

    void clearMouseIndicators();
    void removeMouseIndicatorsFromScenes();
    void updateMouseIndicators();
    void updateMouseIndicators(SessionView::MouseIndicatorPosition const& position);

    void adaptViewToWindow();

    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    void initializeImageView(ImageView& imageView);

    std::vector<ImageView*> tightestImageViews(double tolerance = 0);
    ImageView* selectImageViewForFitToView();

private:
    std::unique_ptr<Ui::SessionView> ui;
    Session* session;
    std::vector<SourceImageView*> imageViews;

    std::optional<MouseIndicatorPosition> lastMouseIndicatorPosition;
    std::vector<MouseIndicatorGraphicsItem*> mouseIndicatorItems;

    bool sourceImagesVisible = true;
    bool mouseIndicatorsVisible = true;
    bool mouseIndicatorUpdateBlocked = false;
    bool autoFitInView = true;
};
