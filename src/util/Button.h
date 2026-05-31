#pragma once

// menu button types
enum class ButtonType
{
    Text,
    MenuIcon
};

// button struct for menu options
struct Button
{
    ButtonType type;

    sf::RectangleShape background;          // common background for all buttons
    sf::Text text;                          // used for text buttons
    std::vector<sf::RectangleShape> bars;   // used for menu icon

    // constructor for both text buttons and menu icon button
    Button(ButtonType t,
        const sf::Font& font,
        const std::string& label,
        sf::Vector2f size,
        sf::Vector2f position)
        : type(t),
        text(font, label, 18)
    {
        // common setup for all buttons
        background.setSize(size);
        background.setPosition(position);
        background.setFillColor(sf::Color(50, 50, 50));

        if (type == ButtonType::Text)
        {
            text.setPosition(position + sf::Vector2f(10.f, 8.f));   // small padding for text
        }
        else if (type == ButtonType::MenuIcon)
        {
            float barWidth = size.x * 0.6f; // bars are 60% of button width
            float barHeight = 3.f;          // bar height is fixed
            float spacing = 8.f;            // space between bars

            // create horizontal bars for menu icon
            for (int i = 0; i < 3; ++i)
            {
                sf::RectangleShape bar;
                bar.setSize({ barWidth, barHeight });
                bar.setFillColor(sf::Color::White);

                bar.setPosition(position + sf::Vector2f(
                    size.x * 0.2f,
                    size.y * 0.3f + i * spacing
                ));

                bars.push_back(bar);
            }
        }
    }

    bool isHovered(sf::Vector2f mousePos) const
    {
        return background.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window) const
    {
        window.draw(background);

        if (type == ButtonType::Text)
        {
            window.draw(text);
        }
        else if (type == ButtonType::MenuIcon)
        {
            for (const auto& bar : bars)
                window.draw(bar);
        }
    }

    void updateHover(sf::Vector2f mousePos)
    {
        if (isHovered(mousePos))
            background.setFillColor(sf::Color(80, 80, 80));
        else
            background.setFillColor(sf::Color(50, 50, 50));
    }

    void setPosition(sf::Vector2f position)
    {
        sf::Vector2f delta =
            position -
            background.getPosition();

        background.move(delta);

        if (type == ButtonType::Text)
        {
            text.move(delta);
        }
        else if (type == ButtonType::MenuIcon)
        {
            for (auto& bar : bars)
                bar.move(delta);
        }
    }
};