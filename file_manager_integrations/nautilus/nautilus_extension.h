#pragma once

#include <memory>

extern "C" {
#include <glib-object.h>
}

class Executor;

class Extension {
public:
    explicit Extension(GTypeModule* module);
    ~Extension();

    GType registeredExtensionType() const;

public:
    std::unique_ptr<Executor> executor;

private:
    GType type;
};

Extension& staticExtensionObject(GTypeModule* module);
Extension& staticExtensionObject();
void cleanupStaticExtensionObject();
