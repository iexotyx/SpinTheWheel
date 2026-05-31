#pragma once

#include <filesystem>

#include "WheelFile.h"
#include "TreeFile.h"
#include "util/Button.h"

enum class FileDialogMode
{
    LoadWheel,
    SaveWheel,
    LoadTree,
    SaveTree
};

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

    if (browser.selectedIndex >=
        browser.entries.size())
        return false;

    return WheelFile::load(
        browser.entries[
            browser.selectedIndex
        ].path(),
                wheel);
}

inline bool deleteSelectedWheel(
    FileBrowserState& browser)
{
    if (browser.selectedIndex < 0)
        return false;

    if (browser.selectedIndex >=
        static_cast<int>(browser.entries.size()))
        return false;

    try
    {
        std::filesystem::remove(
            browser.entries[
                browser.selectedIndex
            ].path());

        refreshFileBrowser(browser);

        browser.selectedIndex = -1;

        return true;
    }
    catch (...)
    {
        return false;
    }
}

inline bool saveEditorWheel(
    const EditorState& editor,
    Wheel& wheel,
    const std::filesystem::path& wheelDirectory)
{
    Wheel tempWheel;

    tempWheel.segments =
        editor.segments;

    tempWheel.name =
        editor.wheelName.empty()
        ? "Untitled Wheel"
        : editor.wheelName;

    std::filesystem::create_directories(
        wheelDirectory);

    const std::filesystem::path savePath =
        wheelDirectory /
        (tempWheel.name + ".csv");

    wheel = tempWheel;

    return WheelFile::save(
        tempWheel,
        savePath);
}