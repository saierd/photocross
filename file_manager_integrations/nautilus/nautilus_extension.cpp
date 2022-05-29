#include "nautilus_extension.h"

#include "executor.h"

#include <memory>
#include <string>
#include <vector>

extern "C" {
#include <nautilus-extension.h>
}

struct ExtensionClass {
    GObjectClass parentSlot;
    Extension* extension;
};

struct ExtensionInstance {
    GObject parentSlot;
};

void extensionClassInitializer(gpointer /* class */, gpointer /* data */) {}
void extensionInstanceInitializer(GTypeInstance* /* instance */, gpointer /* class */) {}

void fileItemCallback(NautilusMenuItem* /* item */, gpointer data)
{
    int id = static_cast<int>(reinterpret_cast<ptrdiff_t>(data));
    staticExtensionObject().executor->executeMenuEntry(id);
}

GList* getFileItems(NautilusMenuProvider* /* provider */, GtkWidget* /* window */, GList* files)
{
    auto& executor = *staticExtensionObject().executor;

    std::vector<Executor::FileInfo> selectedFiles;

    for (GList* fileListEntry = files; fileListEntry != nullptr; fileListEntry = fileListEntry->next) {
        NautilusFileInfo* file = NAUTILUS_FILE_INFO(fileListEntry->data);
        if (nautilus_file_info_is_directory(file) != 0) {
            continue;
        }

        char* uri = nautilus_file_info_get_uri(file);
        if (uri == nullptr) {
            continue;
        }
        char* path = g_filename_from_uri(uri, nullptr, nullptr);
        g_free(uri);
        if (path == nullptr) {
            // Not a local file.
            continue;
        }

        char* mimeType = nautilus_file_info_get_mime_type(file);

        selectedFiles.push_back({path, mimeType});

        g_free(path);
        g_free(mimeType);
    }

    GList* menuItems = nullptr;
    for (auto const& menuEntry : executor.getMenuEntries(selectedFiles)) {
        std::string entryId = "NautilusPhotoCross::" + std::to_string(menuEntry.id);

        NautilusMenuItem* item =
            nautilus_menu_item_new(entryId.c_str(), menuEntry.caption.c_str(), menuEntry.description.c_str(), nullptr);
        g_signal_connect(item, "activate", G_CALLBACK(fileItemCallback), reinterpret_cast<void*>(menuEntry.id));

        menuItems = g_list_append(menuItems, item);
    }

    return menuItems;
}

void initializeExtensionInterface(void* _interface, void* /* data */)
{
    auto* interface = reinterpret_cast<NautilusMenuProviderIface*>(_interface);
    interface->get_file_items = getFileItems;
}

Extension::Extension(GTypeModule* module)
{
    static GTypeInfo const typeInfo = {sizeof(ExtensionClass),
                                       nullptr,
                                       nullptr,
                                       extensionClassInitializer,
                                       nullptr,
                                       nullptr,
                                       sizeof(ExtensionInstance),
                                       0,
                                       extensionInstanceInitializer,
                                       nullptr};

    static const GInterfaceInfo menuProviderInterfaceInfo = {initializeExtensionInterface, nullptr, nullptr};

    type = g_type_module_register_type(module,
                                       G_TYPE_OBJECT,
                                       "nautilus-photocross",
                                       &typeInfo,
                                       static_cast<GTypeFlags>(0));
    g_type_module_add_interface(module, type, NAUTILUS_TYPE_MENU_PROVIDER, &menuProviderInterfaceInfo);

    executor = std::make_unique<Executor>();
}

Extension::~Extension() = default;

GType Extension::registeredExtensionType() const
{
    return type;
}

static std::unique_ptr<Extension> extension;

Extension& staticExtensionObject(GTypeModule* module)
{
    if (!extension) {
        extension = std::make_unique<Extension>(module);
    }
    return *extension;
}

Extension& staticExtensionObject()
{
    return *extension;
}

void cleanupStaticExtensionObject()
{
    extension.reset();
}
