#pragma once

#include <QObject>

class SignalBlocker {
public:
    SignalBlocker(QObject* _object)
      : object(_object)
    {
        if (object) {
            object->blockSignals(true);
        }
    }

    ~SignalBlocker()
    {
        if (object) {
            object->blockSignals(false);
        }
    }

private:
    QObject* object;
};
