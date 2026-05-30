#pragma once

// draw editable text function
void drawEditableText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    sf::Vector2f position,
    bool active,
    bool textSelected,
    bool cursorVisible,
    float maxWidth = 0.f,
    unsigned int characterSize = 18)
{
    sf::Text drawable(font);

    std::string visibleText = text;

    drawable.setCharacterSize(characterSize);

    // Keep the right-most text visible
    if (maxWidth > 0.f)
    {
        std::size_t firstVisible = 0;

        drawable.setString(text);

        if (drawable.getLocalBounds().size.x > maxWidth)
        {
            while (firstVisible < text.size())
            {
                drawable.setString(text.substr(firstVisible));

                if (drawable.getLocalBounds().size.x <= maxWidth)
                    break;

                ++firstVisible;
            }

            visibleText = text.substr(firstVisible);
        }
    }

    drawable.setString(visibleText);
    drawable.setFillColor(sf::Color::Black);
    drawable.setPosition(position);

    // selection highlight
    if (active && textSelected)
    {
        sf::FloatRect bounds = drawable.getGlobalBounds();

        sf::RectangleShape selection({
            bounds.size.x + 6.f,
            bounds.size.y + 4.f
            });

        selection.setPosition({
            bounds.position.x - 3.f,
            bounds.position.y - 2.f
            });

        selection.setFillColor(
            sf::Color(180, 180, 180, 180)
        );

        window.draw(selection);
    }

    window.draw(drawable);

    // cursor
    if (active &&
        cursorVisible &&
        !textSelected)
    {
        sf::FloatRect bounds =
            drawable.getGlobalBounds();

        sf::RectangleShape cursor(
            { 2.f, bounds.size.y }
        );

        cursor.setFillColor(sf::Color::Black);

        float cursorX = bounds.position.x;

        if (!visibleText.empty())
            cursorX += bounds.size.x;

        cursor.setPosition({
            cursorX + 2.f,
            bounds.position.y
            });

        window.draw(cursor);
    }
}