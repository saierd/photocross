#pragma once

#include <QMainWindow>

#include <memory>

namespace Ui {
class MainWindow;
}

class Session;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    Session& getSession();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void openImages();

    void updateLayoutDirection(bool layoutIsHorizontal);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<Session> session;
};
