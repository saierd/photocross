#include "executor.h"

#include <functional>
#include <iostream>
#include <map>

namespace {

std::string const applicationName = "PhotoCross";
std::string const applicationCommand = "photocross";

}  // namespace

class Executor::Impl {
public:
    std::map<int, std::function<void()>> currentEntryCallbacks;
    std::string selectedFile;

    void launch(std::vector<std::string> const& files)
    {
        selectedFile = "";

        std::string command = applicationCommand;
        for (auto const& file : files) {
            command += " \"" + file + "\"";
        }
        if (std::system(command.c_str()) != 0) {
            std::cerr << "Failed to launch " << applicationName << std::endl;
        }
    }

    class MenuEntryBuilder;
};

class Executor::Impl::MenuEntryBuilder {
public:
    explicit MenuEntryBuilder(Executor::Impl* _impl)
      : impl(_impl)
    {
    }

    void add(std::string const& description, std::function<void()> callback)
    {
        impl->currentEntryCallbacks[nextId] = std::move(callback);
        entries.push_back({nextId, description, description});
        nextId++;
    }

    std::vector<Executor::MenuEntry> getEntries()
    {
        return std::move(entries);
    }

private:
    Executor::Impl* impl;

    std::vector<Executor::MenuEntry> entries;
    int nextId = 0;
};

bool Executor::FileInfo::isImageFile() const
{
    return mimeType.rfind("image/", 0) == 0;
}

Executor::Executor()
{
    impl = std::make_unique<Impl>();
}

Executor::~Executor() = default;

std::vector<Executor::MenuEntry> Executor::getMenuEntries(std::vector<FileInfo> const& selectedFiles) const
{
    impl->currentEntryCallbacks.clear();

    std::vector<std::string> files;
    for (auto const& info : selectedFiles) {
        if (info.isImageFile()) {
            files.push_back(info.path);
        }
    }
    if (files.empty()) {
        return {};
    }

    Impl::MenuEntryBuilder menuEntries(impl.get());

    if (!impl->selectedFile.empty()) {
        std::string description = applicationName + ": Compare with '" + impl->selectedFile + "'";

        std::vector<std::string> filesForComparison = files;
        filesForComparison.insert(filesForComparison.begin(), impl->selectedFile);
        menuEntries.add(description, [this, filesForComparison = std::move(filesForComparison)]() {
            impl->launch(filesForComparison);
        });
    }

    if (files.size() == 1) {
        std::string description = applicationName + ": Select for Comparison";
        menuEntries.add(description, [this, file = files[0]]() {
            impl->selectedFile = file;
        });
    } else {
        std::string description = applicationName + ": Compare Images";
        menuEntries.add(description, [this, files = std::move(files)]() {
            impl->launch(files);
        });
    }

    return menuEntries.getEntries();
}

void Executor::executeMenuEntry(int id)
{
    auto it = impl->currentEntryCallbacks.find(id);
    if (it == impl->currentEntryCallbacks.end()) {
        return;
    }

    it->second();
}
