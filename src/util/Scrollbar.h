#pragma once

// handle scroll input function
void handleScrollInput(
    float scrollDelta,
    int& scrollOffset,
    int totalItems,
    int visibleItems
)
{
    scrollOffset -= static_cast<int>(scrollDelta);

    int maxOffset =
        std::max(
            0,
            totalItems - visibleItems
        );

    if (scrollOffset < 0)
        scrollOffset = 0;

    if (scrollOffset > maxOffset)
        scrollOffset = maxOffset;
}

// draw scrollbar function
void drawScrollbar(
    sf::RenderWindow& window,
    float x,
    float y,
    float height,
    int visibleItems,
    int totalItems,
    int scrollOffset)
{
    if (totalItems <= visibleItems)
        return;

    constexpr float scrollbarWidth = 8.f;

    int maxOffset =
        std::max(0, totalItems - visibleItems);

    float scrollFraction =
        (maxOffset > 0)
        ? static_cast<float>(scrollOffset)
        / static_cast<float>(maxOffset)
        : 0.f;

    // track
    sf::RectangleShape track({
        scrollbarWidth,
        height
        });

    track.setPosition({ x, y });
    track.setFillColor(
        sf::Color(210, 210, 210)
    );

    window.draw(track);

    // thumb
    float thumbHeight =
        height *
        (static_cast<float>(visibleItems)
            / static_cast<float>(totalItems));

    float thumbY =
        y +
        (height - thumbHeight)
        * scrollFraction;

    sf::RectangleShape thumb({
        scrollbarWidth,
        thumbHeight
        });

    thumb.setPosition({
        x,
        thumbY
        });

    thumb.setFillColor(
        sf::Color(120, 120, 120)
    );

    window.draw(thumb);
}