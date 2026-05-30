#pragma once

#include <filesystem>

#include "WheelFile.h"
#include "TreeFile.h"

enum class FileDialogMode
{
    LoadWheel,
    SaveWheel,
    LoadTree,
    SaveTree
};

enum class FileDialogMode;

struct FileBrowserState
{
    std::filesystem::path currentDirectory;

    std::filesystem::path selectedFile;

    FileDialogMode mode =
        FileDialogMode::LoadWheel;

    std::vector<std::filesystem::directory_entry>
        entries;

    int selectedIndex = -1;

    int scrollOffset = 0;
};

void saveWheelToFile(
    const Wheel& wheel,
    const std::filesystem::path& path);

Wheel loadWheelFromFile(
    const std::filesystem::path& path);

void refreshFileBrowser(
    FileBrowserState& browser)
{
    browser.entries.clear();

    if (!std::filesystem::exists(
        browser.currentDirectory))
    {
        return;
    }

    for (const auto& entry :
        std::filesystem::directory_iterator(
            browser.currentDirectory))
    {
        if (entry.is_regular_file())
        {
            browser.entries.push_back(
                entry);
        }
    }

    std::sort(
        browser.entries.begin(),
        browser.entries.end(),
        [](const auto& a,
            const auto& b)
        {
            return
                a.path().filename().string()
                <
                b.path().filename().string();
        });
}

bool loadSelectedWheel(
    const FileBrowserState& browser,
    Wheel& wheel)
{
    if (browser.selectedIndex < 0)
        return false;

    return WheelFile::load(
        browser.selectedFile,
        wheel);
}