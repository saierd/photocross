#pragma once

#include "widgets/image_view.h"

class Session;
class ComparisonSettings;

class ComparisonView : public ImageView {
public:
    explicit ComparisonView(QWidget* parent = nullptr);

    void update(Session const& session, ComparisonSettings const& settings);
};
