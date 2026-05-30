#pragma once

#include "persistence/FileBrowser.h"

void drawFileBrowserUI(
    sf::RenderWindow& window,
    const sf::Font& font,
    const FileBrowserState& browser)
{
	// title
    TableLayout layout =
        getTableLayout(window);

    sf::Text title(font);

    title.setString(
        "Load Wheel");

    title.setCharacterSize(30);

    title.setPosition({
        layout.startX,
        layout.startY - 60.f
        });

    title.setFillColor(
        sf::Color::Black);

    window.draw(title);

	// list area dimensions
    const float startX =
        layout.startX;

    const float startY =
        layout.startY;

    constexpr float rowHeight = 40.f;

    constexpr float width = 500.f;

    // rows
    for (int i = 0;
        i < browser.entries.size();
        ++i)
    {
        float y =
            startY +
            i * rowHeight;

        sf::RectangleShape row({
            width,
            rowHeight - 2.f
            });

        row.setPosition({
            startX,
            y
            });

        row.setFillColor(
            i == browser.selectedIndex
            ?
            sf::Color(200, 220, 255)
            :
            sf::Color::White
        );

        row.setOutlineThickness(1.f);

        row.setOutlineColor(
            sf::Color(180, 180, 180));

        window.draw(row);

        sf::Text text(font);

        text.setCharacterSize(18);

        text.setFillColor(
            sf::Color::Black);

        text.setString(
            browser.entries[i]
            .path()
            .stem()
            .string());

        text.setPosition({
            startX + 8.f,
            y + 8.f
            });

        window.draw(text);
    }
}