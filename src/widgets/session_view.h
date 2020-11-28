#pragma once

#include <QWidget>

#include <memory>

namespace Ui {
class SessionView;
}

class Session;

class SessionView : public QWidget {
    Q_OBJECT

public:
    explicit SessionView(QWidget* parent = nullptr);
    ~SessionView() override;

    void setSession(Session* session);

public slots:
    void fitToView();

private:
    std::unique_ptr<Ui::SessionView> ui;
    Session* session;
};
