#pragma once

#include <QWidget>

#include <memory>
#include <vector>

namespace Ui {
class SessionView;
}

class ImageView;
class Session;

class SessionView : public QWidget {
    Q_OBJECT

public:
    explicit SessionView(QWidget* parent = nullptr);
    ~SessionView() override;

    void setSession(Session* session);

    bool getAutoFitInView() const;
    void setAutoFitInView(bool enable);

signals:
    void autoFitInViewChanged(bool autoFitInView);

public slots:
    void flipLayoutDirection();

    void zoomIn();
    void zoomOut();
    void fitToView();

private slots:
    void updateImages();
    void updateComparisonView();

    void adaptViewToWindow();

    void resizeEvent(QResizeEvent* event) override;

private:
    std::unique_ptr<Ui::SessionView> ui;
    Session* session;
    std::vector<ImageView*> imageViews;

    bool autoFitInView = true;
};
