#pragma once

#include <filesystem>

struct AppSettings
{
    std::filesystem::path wheelDirectory =
        "Wheels";

    std::filesystem::path treeDirectory =
        "Trees";
};