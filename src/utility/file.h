#pragma once

#include <QString>
#include <QStringList>

class QWidget;

bool isImageFile(QString const& file);

QString selectImageFile(QWidget* parent);
QStringList selectImageFiles(QWidget* parent);

QString selectImageFileForSaving(QString const& defaultFileName, QWidget* parent);