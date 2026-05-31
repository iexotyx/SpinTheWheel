#pragma once

struct VisibleRange
{
    int maxRows;
    int startRow;
    int endRow;
};

inline VisibleRange calculateVisibleRange(
    const sf::RenderWindow& window,
    float startY,
    float rowHeight,
    int totalRows,
    int scrollOffset)
{
    VisibleRange range;

    range.maxRows =
        static_cast<int>(
            (window.getSize().y -
                startY -
                140.f)
            / rowHeight);

    range.startRow =
        std::min(
            scrollOffset,
            std::max(
                0,
                totalRows - range.maxRows));

    range.endRow =
        std::min(
            range.startRow +
            range.maxRows,
            totalRows);

    return range;
}

inline void drawTableRowBackground(
    sf::RenderWindow& window,
    const sf::FloatRect& rect,
    bool selected)
{
    sf::RectangleShape row(rect.size);

    row.setPosition(rect.position);

    row.setFillColor(
        selected
        ? sf::Color(200, 220, 255)
        : sf::Color::White);

    row.setOutlineThickness(1.f);

    row.setOutlineColor(
        sf::Color(180, 180, 180));

    window.draw(row);
}