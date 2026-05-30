#pragma once

// draw menu button function
void drawMenuButton(sf::RenderWindow& window, const Button& button)
{
    button.draw(window);
}

// draw menu function
void drawMenu(sf::RenderWindow& window,
    const std::vector<Button>& options,
    const sf::Font& font)
{
    for (const auto& btn : options)
        btn.draw(window);
}