#pragma once

#include <memory>
#include <string>
#include <vector>

class Executor {
public:
    struct FileInfo {
        std::string path;
        std::string mimeType;

        bool isImageFile() const;
    };

    struct MenuEntry {
        int id;

        std::string caption;
        std::string description;
    };

    Executor();
    ~Executor();

    std::vector<MenuEntry> getMenuEntries(std::vector<FileInfo> const& selectedFiles) const;
    void executeMenuEntry(int id);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
