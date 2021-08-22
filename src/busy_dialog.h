#pragma once

#include <functional>

class QString;
class QWidget;

void runWithBusyDialog(QString const& text, std::function<void()> const& f, QWidget* parent = nullptr);
