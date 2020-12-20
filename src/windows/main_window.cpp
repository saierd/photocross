#include "main_window.h"

#include "session.h"

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
    ui = std::make_unique<Ui::MainWindow>();
    ui->setupUi(this);

    connect(ui->actionZoomIn, &QAction::triggered, ui->session, &SessionView::zoomIn);
    connect(ui->actionZoomOut, &QAction::triggered, ui->session, &SessionView::zoomOut);

    connect(ui->actionAutoFitToView, &QAction::triggered, ui->session, &SessionView::fitToView);
    connect(ui->session, &SessionView::autoFitInViewChanged, ui->actionAutoFitToView, &QAction::setChecked);
    connect(ui->session, &SessionView::autoFitInViewChanged, ui->actionAutoFitToView, &QAction::setDisabled);

    connect(ui->actionFlipLayoutDirection, &QAction::triggered, ui->session, &SessionView::flipLayoutDirection);

    session = std::make_unique<Session>();
    ui->session->setSession(session.get());
}

MainWindow::~MainWindow() = default;

Session& MainWindow::getSession() &
{
    return *session;
}
