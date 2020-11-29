#include "main_window.h"

#include "session.h"

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
    ui = std::make_unique<Ui::MainWindow>();
    ui->setupUi(this);

    connect(ui->actionFitToView, &QAction::triggered, ui->session, &SessionView::fitToView);
    connect(ui->actionFlipLayoutDirection, &QAction::triggered, ui->session, &SessionView::flipLayoutDirection);

    std::vector<Image> images = {Image("test_data/1.png"), Image("test_data/2.png")};
    session = std::make_unique<Session>(std::move(images));
    ui->session->setSession(session.get());
}

MainWindow::~MainWindow() = default;
