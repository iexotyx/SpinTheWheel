#pragma once

#include "persistence/FileBrowser.h"
#include "util/Table.h"

void drawFileBrowserTitle(
    sf::RenderWindow& window,
    const sf::Font& font,
    const TableLayout& layout)
{
    sf::Text title(font);

    title.setString("Load Wheel");

    title.setCharacterSize(30);

    title.setPosition({
        layout.startX,
        layout.startY - 60.f
        });

    title.setFillColor(
        sf::Color::Black);

    window.draw(title);
}

void drawFileBrowserRows(
    sf::RenderWindow& window,
    const sf::Font& font,
    const FileBrowserState& browser,
    const TableLayout& layout,
    int startRow,
    int endRow)
{
    for (int i = startRow;
        i < endRow;
        ++i)
    {
        float y =
            layout.startY +
            (i - startRow) *
            layout.rowHeight;

        sf::FloatRect rowRect(
            {
                layout.startX,
                y
            },
            {
                layout.tableWidth,
                layout.rowHeight - 2.f
            });

            drawTableRowBackground(
                window,
                rowRect,
                i == browser.selectedIndex);

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
                layout.startX + 8.f,
                y + 8.f
                });

            window.draw(text);
    }
}

void drawFileBrowserUI(
    sf::RenderWindow& window,
    const sf::Font& font,
    const FileBrowserState& browser,
    TableLayout& layout,
    Button& loadWheelButton,
    Button& deleteWheelButton)
{
    drawFileBrowserTitle(
        window,
        font,
        layout);

    VisibleRange range =
        calculateVisibleRange(
            window,
            layout.startY,
            layout.rowHeight,
            static_cast<int>(
                browser.entries.size()),
            browser.scrollOffset);

    float tableHeight =
        range.maxRows *
        layout.rowHeight;

    drawFileBrowserRows(
        window,
        font,
        browser,
        layout,
        range.startRow,
        range.endRow);

    drawScrollbar(
        window,
        layout.startX +
        layout.tableWidth +
        10.f,
        layout.startY,
        range.maxRows *
        layout.rowHeight,
        range.maxRows,
        static_cast<int>(
            browser.entries.size()),
        browser.scrollOffset);

    updateFileBrowserButtonPos(
        layout,
        tableHeight,
        loadWheelButton,
        deleteWheelButton);

    if (browser.selectedIndex >= 0)
    {
        loadWheelButton.draw(window);
        deleteWheelButton.draw(window);
    }
}