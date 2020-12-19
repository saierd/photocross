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

public slots:
    void flipLayoutDirection();

    void fitToView();

private slots:
    void updateImages();
    void updateComparisonView();

private:
    std::unique_ptr<Ui::SessionView> ui;
    Session* session;
    std::vector<ImageView*> imageViews;
};
