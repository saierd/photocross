#pragma once

#include "window.h"

#include <memory>

namespace Ui {
class MainWindow;
}

class Session;

class MainWindow final : public Window {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    Session& getSession();

private slots:
    void openImages();

    void updateLayoutDirection(bool layoutIsHorizontal);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<Session> session;
};
