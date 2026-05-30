#pragma once

#include <filesystem>

struct DecisionTreeNode
{
    std::string label;

    std::filesystem::path wheelFile;

    std::vector<DecisionTreeNode> children;
};

struct DecisionTree
{
    std::string name;

    std::vector<DecisionTreeNode> roots;
};