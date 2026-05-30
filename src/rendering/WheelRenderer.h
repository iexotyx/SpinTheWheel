#pragma once

// draw wheel function
void drawWheel(sf::RenderWindow& window,
    const std::vector<Segment>& segments,
    float radius,
    sf::Vector2f center,
    float rotation,
    const sf::Font& font
)
{
    float totalWeight = 0.f;
    for (const auto& s : segments)
        totalWeight += s.weight;

    float currentAngle = rotation;

    for (const auto& s : segments)
    {
        float angle = (s.weight / totalWeight) * 360.f;

        // lock angles for this segment
        float segmentStart = currentAngle;
        float midAngle = segmentStart + angle * 0.5f;

        // draw slice
        sf::VertexArray slice(sf::PrimitiveType::TriangleFan);

        sf::Vertex centerVertex;
        centerVertex.position = center;
        centerVertex.color = s.colour;
        slice.append(centerVertex);

        int points = 128;

        for (int i = 0; i <= points; ++i)
        {
            float current = segmentStart + angle * (float(i) / points);
            float rad = current * PI / 180.f;

            sf::Vector2f point = {
                center.x + std::cos(rad) * radius,
                center.y + std::sin(rad) * radius
            };

            sf::Vertex v;
            v.position = point;
            v.color = s.colour;
            slice.append(v);
        }

        window.draw(slice);

        // draw labels
        float rad = midAngle * PI / 180.f;

        float textRadius = radius * 0.5f;        // place text in true visual center of segment

        sf::Vector2f textPos = {
            center.x + std::cos(rad) * textRadius,
            center.y + std::sin(rad) * textRadius
        };

        sf::Text text(font);
        text.setString(s.label);

        // base size of text relative to wheel size, will be scaled down if it doesn't fit
        unsigned int charSize = static_cast<unsigned int>(radius * 0.2f);
        text.setCharacterSize(charSize);

        // measure text bounds to check if it fits within the segment
        sf::FloatRect bounds = text.getLocalBounds();

        // available width along arc
        float arcLength = (angle * PI / 180.f) * textRadius;
        float maxWidth = arcLength * 0.75f;

        // Scale down if needed
        if (bounds.size.x > maxWidth)
        {
            float scale = maxWidth / bounds.size.x;
            text.setCharacterSize(static_cast<unsigned int>(charSize * scale));
        }

        // Recalculate bounds after resizing
        bounds = text.getLocalBounds();

        // Proper centering
        text.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });

        text.setPosition(textPos);

        // Affix text to the segment
        text.setRotation(sf::degrees(midAngle + 180.f));

        // Contrast-aware colour
        float brightness =
            0.299f * s.colour.r +
            0.587f * s.colour.g +
            0.114f * s.colour.b;

        text.setFillColor(brightness > 128.f ? sf::Color::Black : sf::Color::White);

        window.draw(text);

        // Advance to next segment
        currentAngle += angle;
    }
}

// draw pointer function
sf::Vector2f drawPointer(sf::RenderWindow& window,
    float radius,
    const sf::Vector2f& center)
{
    float height = radius * 0.12f;
    float width = radius * 0.08f;

    // tip offset into the wheel
    float overlap = radius * 0.05f;

    sf::Vector2f tip = {
        center.x,
        center.y - radius + overlap
    };

    sf::VertexArray pointer(sf::PrimitiveType::Triangles, 3);

    // Tip
    pointer[0].position = tip;

    // Base
    pointer[1].position = tip + sf::Vector2f(-width * 0.6f, -height);
    pointer[2].position = tip + sf::Vector2f(width * 0.6f, -height);

    for (int i = 0; i < 3; ++i)
        pointer[i].color = sf::Color::Black;

    window.draw(pointer);

    return tip;
}

// draw wheel name function
void drawWheelName(const sf::Font& font,
    const Wheel& wheel,
    sf::RenderWindow& window,
    const sf::Vector2f& center)
{
    sf::Text title(font);
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::Black);
    title.setString(wheel.name);

    // center horizontally
    sf::FloatRect bounds = title.getGlobalBounds();

    title.setPosition({ center.x - bounds.size.x * 0.5f, 20.f });

    window.draw(title);
}
void updateTooltipPosition(
    sf::Text& tooltip,
    float radius,
    const sf::Vector2f& center)
{
        tooltip.setPosition({
            center.x - tooltip.getGlobalBounds().size.x * 0.5f,
            center.y + radius + 30.f
            });
}