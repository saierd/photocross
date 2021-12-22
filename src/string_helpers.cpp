#include "string_helpers.h"

QString commonPrefix(QString const& a, QString const& b)
{
    int maxCommonIndex = 0;
    while (maxCommonIndex < std::min(a.size(), b.size()) && a[maxCommonIndex] == b[maxCommonIndex]) {
        maxCommonIndex++;
    }

    return a.left(maxCommonIndex);
}
