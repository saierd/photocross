#include "session_view.h"

#include "session.h"

#include "ui_session_view.h"

SessionView::SessionView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::SessionView>();
    ui->setupUi(this);

    ui->comparisonView->setCaption("Comparison View");

    connect(ui->splitter, &QSplitter::splitterMoved, [this]() {
        ui->comparisonView->forceViewPropagation();
    });

    ui->image1->synchronizeViews(*ui->image2);
    ui->image1->synchronizeViews(*ui->comparisonView);
    ui->image2->synchronizeViews(*ui->comparisonView);
}

SessionView::~SessionView() = default;

void SessionView::setSession(Session* _session)
{
    session = _session;

    auto updateImage1 = [this]() {
        ui->image1->setCaption(session->image1().file());

        ui->image1->clear();
        ui->image1->addPixmap(session->image1().toPixmap());
    };

    auto updateImage2 = [this]() {
        ui->image2->setCaption(session->image2().file());

        ui->image2->clear();
        ui->image2->addPixmap(session->image2().toPixmap());
    };

    auto updateComparisonView = [this]() {
        ui->comparisonView->clear();
        ui->comparisonView->addPixmap(session->image1().toGrayscalePixmap());
        ui->comparisonView->addPixmap(QPixmap::fromImage(session->comparisonImage()), 0.7);
    };

    connect(session, &Session::image1Changed, updateImage1);
    connect(session, &Session::image2Changed, updateImage2);

    connect(session, &Session::image1Changed, updateComparisonView);
    connect(session, &Session::image2Changed, updateComparisonView);

    updateImage1();
    updateImage2();
    updateComparisonView();
}

void SessionView::flipLayoutDirection()
{
    if (ui->splitter->orientation() == Qt::Horizontal) {
        ui->splitter->setOrientation(Qt::Vertical);
        ui->imagesLayout->setDirection(QBoxLayout::LeftToRight);
    } else {
        ui->splitter->setOrientation(Qt::Horizontal);
        ui->imagesLayout->setDirection(QBoxLayout::TopToBottom);
    }

    // Flipping the layout might change the center point visible in the image if the image does not fit the graphics
    // view. Force synchronization of the views so that they are all centered on the same point again.
    QApplication::processEvents();
    ui->comparisonView->forceViewPropagation();
}

void SessionView::fitToView()
{
    // TODO: Fit in smallest image view.
    ui->image1->fitViewToScene();
}
