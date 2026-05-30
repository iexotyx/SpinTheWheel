#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>

namespace WheelFile
{
    bool save(
        const Wheel& wheel,
        const std::filesystem::path& path);

    bool load(
        const std::filesystem::path& path,
        Wheel& wheel);
}

bool WheelFile::save(
    const Wheel& wheel,
    const std::filesystem::path& path)
{
    std::ofstream file(path);

    if (!file.is_open())
        return false;

    file << "Wheel Title," << wheel.name << '\n';

    file << "Label,Weight,R,G,B\n";

    for (const auto& segment : wheel.segments)
    {
        file
            << segment.label << ','
            << segment.weight << ','
            << static_cast<int>(segment.colour.r) << ','
            << static_cast<int>(segment.colour.g) << ','
            << static_cast<int>(segment.colour.b)
            << '\n';
    }

    return true;
}

bool WheelFile::load(
    const std::filesystem::path& path,
    Wheel& wheel)
{
    std::ifstream file(path);

    if (!file.is_open())
        return false;

    wheel.segments.clear();

    std::string line;

    if (!std::getline(file, line))
        return false;

    {
        std::stringstream ss(line);

        std::string discard;

        std::getline(ss, discard, ',');
        std::getline(ss, wheel.name);
    }

    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        Segment segment;

        std::string weight;
        std::string r;
        std::string g;
        std::string b;

        std::getline(ss, segment.label, ',');
        std::getline(ss, weight, ',');
        std::getline(ss, r, ',');
        std::getline(ss, g, ',');
        std::getline(ss, b, ',');

        segment.weight = std::stof(weight);

        segment.colour = sf::Color(
            static_cast<std::uint8_t>(std::stoi(r)),
            static_cast<std::uint8_t>(std::stoi(g)),
            static_cast<std::uint8_t>(std::stoi(b))
        );

        wheel.segments.push_back(segment);
    }

    return true;
}