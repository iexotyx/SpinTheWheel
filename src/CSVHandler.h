#pragma once

#include <vector>
#include <string>
#include "Segment.h"

struct Wheel
{
    std::string name;
    std::vector<Segment> segments;
};

class CSVHandler
{
public:
    static bool save(const std::string& filename, const Wheel& wheel);
    static bool load(const std::string& filename, Wheel& wheel);
};