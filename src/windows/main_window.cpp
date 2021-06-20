#include "main_window.h"

#include "file_helpers.h"
#include "session.h"

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
    ui = std::make_unique<Ui::MainWindow>();
    ui->setupUi(this);

    session = std::make_unique<Session>();
    ui->session->setSession(session.get());

    connect(ui->actionOpenImages, &QAction::triggered, this, &MainWindow::openImages);

    connect(ui->actionExit, &QAction::triggered, []() {
        QCoreApplication::quit();
    });

    connect(ui->actionReload, &QAction::triggered, [this]() {
        session->reload();
    });

    connect(ui->actionReloadWhenImageFileChanges, &QAction::toggled, session.get(), &Session::setWatchFiles);
    ui->actionReloadWhenImageFileChanges->setChecked(session->getWatchFiles());
    connect(session.get(), &Session::watchFilesChanged, ui->actionReloadWhenImageFileChanges, &QAction::setChecked);

    connect(ui->actionShowSourceImages, &QAction::toggled, ui->session, &SessionView::setSourceImagesVisible);
    ui->actionShowSourceImages->setChecked(ui->session->getSourceImagesVisible());
    connect(ui->session, &SessionView::sourceImagesVisibleChanged, ui->actionShowSourceImages, &QAction::setChecked);

    connect(ui->actionShowMouseIndicators, &QAction::toggled, ui->session, &SessionView::setMouseIndicatorsVisible);
    ui->actionShowMouseIndicators->setChecked(ui->session->getMouseIndicatorsVisible());
    connect(ui->session,
            &SessionView::mouseIndicatorsVisibleChanged,
            ui->actionShowMouseIndicators,
            &QAction::setChecked);

    connect(ui->actionZoomIn, &QAction::triggered, ui->session, &SessionView::zoomIn);
    connect(ui->actionZoomOut, &QAction::triggered, ui->session, &SessionView::zoomOut);

    connect(ui->actionAutoFitToView, &QAction::triggered, ui->session, &SessionView::fitToView);
    connect(ui->actionFitComparisonImageToView,
            &QAction::triggered,
            ui->session,
            &SessionView::fitComparisonImageToView);
    connect(ui->session, &SessionView::autoFitInViewChanged, ui->actionAutoFitToView, &QAction::setChecked);
    connect(ui->session, &SessionView::autoFitInViewChanged, ui->actionAutoFitToView, &QAction::setDisabled);

    connect(ui->actionRotateImagesLeft, &QAction::triggered, ui->session, &SessionView::rotateImagesLeft);
    connect(ui->actionRotateImagesRight, &QAction::triggered, ui->session, &SessionView::rotateImagesRight);

    connect(ui->actionFlipLayoutDirection, &QAction::triggered, ui->session, &SessionView::flipLayoutDirection);
    updateLayoutDirection(ui->session->getLayoutIsHorizontal());
    connect(ui->session, &SessionView::layoutDirectionChanged, this, &MainWindow::updateLayoutDirection);
}

MainWindow::~MainWindow() = default;

Session& MainWindow::getSession() &
{
    return *session;
}

void MainWindow::openImages()
{
    auto files = selectImageFiles(this);
    if (!files.empty()) {
        getSession().loadImages(files);
    }
}

void MainWindow::updateLayoutDirection(bool layoutIsHorizontal)
{
    if (layoutIsHorizontal) {
        ui->actionFlipLayoutDirection->setIcon(QIcon::fromTheme("rotate-to-vertical"));
    } else {
        ui->actionFlipLayoutDirection->setIcon(QIcon::fromTheme("rotate-to-horizontal"));
    }
}
