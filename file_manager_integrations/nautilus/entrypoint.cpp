#include "nautilus_extension.h"

#include <array>

extern "C" {
#include <nautilus-extension.h>
}

static std::array<GType, 1> extensionTypes;

void nautilus_module_initialize(GTypeModule* module)
{
    extensionTypes[0] = staticExtensionObject(module).registeredExtensionType();
}

void nautilus_module_shutdown()
{
    cleanupStaticExtensionObject();
}

void nautilus_module_list_types(const GType** types, int* num_types)
{
    *types = extensionTypes.data();
    *num_types = extensionTypes.size();
}
