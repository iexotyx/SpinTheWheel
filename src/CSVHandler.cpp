#include "CSVHandler.h"
#include <fstream>
#include <sstream>

bool CSVHandler::save(const std::string& filename, const Wheel& wheel)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    // Metadata row
    file << "name," << wheel.name << "\n";

    // Column headers
    file << "label,weight,r,g,b\n";

    for (const auto& s : wheel.segments)
    {
        file << s.label << ","
            << s.weight << ","
            << static_cast<int>(s.color.r) << ","
            << static_cast<int>(s.color.g) << ","
            << static_cast<int>(s.color.b) << "\n";
    }

    return true;
}

bool CSVHandler::load(const std::string& filename, Wheel& wheel)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

	wheel.segments.clear();             // clear existing segments

    std::string line;
    std::stringstream ss;

	// read metadata line
    if (!std::getline(file, line))
        return false;

    ss.clear();
    ss.str(line);

    std::string key, value;
    std::getline(ss, key, ',');
    std::getline(ss, value, ',');

    if (key == "name")
        wheel.name = value;

    if (!std::getline(file, line))      // if there is no header line, return false
        return false;

	// read segment lines
    while (std::getline(file, line))
    {
        ss.clear();
        ss.str(line);

        std::string token;
        Segment s;

        // label
        std::getline(ss, token, ',');
        s.label = token;

        // weight
        std::getline(ss, token, ',');
        s.weight = std::stof(token);

        // r
        std::getline(ss, token, ',');
        int r = std::stoi(token);

        // g
        std::getline(ss, token, ',');
        int g = std::stoi(token);

        // b
        std::getline(ss, token, ',');
        int b = std::stoi(token);

		s.color = sf::Color(r, g, b);   // create color from RGB values

		wheel.segments.push_back(s);    // add segment to wheel
    }

    return true;
}