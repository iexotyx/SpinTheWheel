#pragma once

#include "EventRouter.h"
#include "util/Table.h"

void handleFileBrowserClick(
    const sf::Vector2f& mousePos,
    const sf::RenderWindow& window,
    FileBrowserState& browser,
    const TableLayout& layout)
{
    browser.selectedIndex = -1;

    VisibleRange range =
        calculateVisibleRange(
            window,
            layout.startY,
            layout.rowHeight,
            static_cast<int>(
                browser.entries.size()),
            browser.scrollOffset);

    for (int i = range.startRow;
        i < range.endRow;
        ++i)
    {
        float y =
            layout.startY +
            (i - range.startRow) *
            layout.rowHeight;

        sf::FloatRect rowRect(
            {
                layout.startX,
                y
            },
            {
                layout.tableWidth,
                layout.rowHeight
            });

            if (rowRect.contains(mousePos))
            {
                browser.selectedIndex = i;
                break;
            }
    }
}

inline void handleFileBrowserScroll(
    float delta,
    const sf::RenderWindow& window,
    const TableLayout& layout,
    FileBrowserState& browser)
{
    int visibleRows =
        static_cast<int>(
            (window.getSize().y -
                layout.startY -
                140.f)
            / layout.rowHeight);

    handleScrollInput(
        delta,
        browser.scrollOffset,
        static_cast<int>(
            browser.entries.size()),
        visibleRows);
}