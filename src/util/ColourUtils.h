#pragma once

// utility function for colour conversion between HSV and RGB for the colour picker UI
sf::Color hsvToRgb(
    float h,
    float s,
    float v
)
{
    float c = v * s;
    float x = c * (1 - std::abs(fmod(h / 60.f, 2) - 1));
    float m = v - c;

    float r = 0, g = 0, b = 0;

    if (h < 60) { r = c; g = x; }
    else if (h < 120) { r = x; g = c; }
    else if (h < 180) { g = c; b = x; }
    else if (h < 240) { g = x; b = c; }
    else if (h < 300) { r = x; b = c; }
    else { r = c; b = x; }

    return sf::Color(
        static_cast<uint8_t>((r + m) * 255),
        static_cast<uint8_t>((g + m) * 255),
        static_cast<uint8_t>((b + m) * 255)
    );
}

// utility function for colour conversion between RGB and HSV for the colour picker UI
void rgbToHsv(const sf::Color& colour, float& h, float& s, float& v)
{
    float r = colour.r / 255.f;
    float g = colour.g / 255.f;
    float b = colour.b / 255.f;

    float max = std::max({ r, g, b });
    float min = std::min({ r, g, b });
    float delta = max - min;

    v = max;

    if (delta < 0.00001f)
    {
        h = 0;
        s = 0;
        return;
    }

    s = delta / max;

    if (max == r)
        h = 60.f * fmod(((g - b) / delta), 6.f);
    else if (max == g)
        h = 60.f * (((b - r) / delta) + 2.f);
    else
        h = 60.f * (((r - g) / delta) + 4.f);

    if (h < 0) h += 360.f;
}

// function used to ensure new segment hues are not too close to existing ones for better visual distinction ---
bool isHueTooClose(float newHue,
    const std::vector<Segment>& segments,
    float minGap = 25.f
)
{
    for (const auto& s : segments)
    {
        float existingHue, sat, val;
        rgbToHsv(s.colour, existingHue, sat, val);

        float diff = std::abs(newHue - existingHue);

        // wrap-around handling
        diff = std::min(diff, 360.f - diff);

        if (diff < minGap)
            return true;
    }
    return false;
}