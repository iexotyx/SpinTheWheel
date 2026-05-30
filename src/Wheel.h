#pragma once

constexpr float PI = 3.14159265f;

struct Segment
{
    std::string label;
    float weight;
    sf::Color colour;
};

// segment struct to hold data for each wheel segment
struct Wheel
{
    std::string name;
    std::vector<Segment> segments;
};

// wheel spin state struct to manage the spinning animation and physics of the wheel
struct WheelSpinState
{
    float rotation = 0.f;
    float angularVelocity = 0.f;
    float deceleration = 0.f;

    bool spinning = false;
};

// Segment ID based on position of the pointer
int getSelectedSegment(const std::vector<Segment>& segments,
    const sf::Vector2f& center,
    const sf::Vector2f& pointerTip,
    float rotation
)
{
    if (segments.empty())
        return -1;

    // Pointer angle in world space
    float dx = pointerTip.x - center.x;
    float dy = pointerTip.y - center.y;

    float pointerAngle = std::atan2(dy, dx) * 180.f / PI;

    if (pointerAngle < 0)
        pointerAngle += 360.f;

    // Total weight
    float totalWeight = 0.f;
    for (const auto& s : segments)
        totalWeight += s.weight;

    // walk segments
    float currentAngle = rotation; // match draw start

    for (int i = 0; i < segments.size(); ++i)
    {
        float segmentAngle = (segments[i].weight / totalWeight) * 360.f;

        float start = std::fmod(currentAngle, 360.f);
        if (start < 0) start += 360.f;

        float end = std::fmod(currentAngle + segmentAngle, 360.f);
        if (end < 0) end += 360.f;

        // handle wrap-around
        if (start < end)
        {
            if (pointerAngle >= start && pointerAngle < end)
                return i;
        }
        else
        {
            // wrapped segment
            if (pointerAngle >= start || pointerAngle < end)
                return i;
        }

        currentAngle += segmentAngle;
    }

    return segments.size() - 1; // fallback
}