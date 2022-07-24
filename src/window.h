#pragma once

#include <QMainWindow>

class Window : public QMainWindow {
public:
    using QMainWindow::QMainWindow;

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    bool wasShown = false;
};