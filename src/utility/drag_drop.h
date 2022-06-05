#pragma once

#include <QStringList>

class QDropEvent;

QStringList extractImageFiles(QDropEvent* event);
