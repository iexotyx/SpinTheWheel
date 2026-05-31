#pragma once

constexpr float PI = 3.14159265f;

// segment struct to hold data for each wheel segment
struct Segment
{
    std::string label;
    float weight;
    sf::Color colour;
};

// wheel struct to hold the name, segments of the wheel, and wheel size
struct Wheel
{
    std::string name;
    std::vector<Segment> segments;

    sf::Vector2f centre;
    float radius;
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
    const sf::Vector2f& centre,
    const sf::Vector2f& pointerTip,
    float rotation
)
{
    if (segments.empty())
        return -1;

    // Pointer angle in world space
    float dx = pointerTip.x - centre.x;
    float dy = pointerTip.y - centre.y;

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

void updateWheelPosition(
    const sf::Vector2u windowSize,
    Wheel& wheel,
    sf::Text& tooltip)
{
    wheel.centre = {
            windowSize.x * 0.5f,
            windowSize.y * 0.5f
    };

    // pointer tip is a fixed distance from the centre
    sf::Vector2f pointerPos = {
        windowSize.x * 0.5f,
        windowSize.y * 0.05f
    };

    wheel.radius = std::min(windowSize.x, windowSize.y) * 0.3f;

    tooltip.setPosition({
            wheel.centre.x - tooltip.getGlobalBounds().size.x * 0.5f,
            wheel.centre.y + wheel.radius + 30.f
        });
}