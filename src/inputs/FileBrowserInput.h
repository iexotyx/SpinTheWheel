#pragma once

void handleFileBrowserClick(
    const sf::Vector2f& mousePos,
    FileBrowserState& browser)
{
    constexpr float startX = 50.f;
    constexpr float startY = 100.f;

    constexpr float rowHeight = 40.f;
    constexpr float width = 500.f;

    browser.selectedIndex = -1;

    for (int i = 0;
        i < browser.entries.size();
        ++i)
    {
        sf::FloatRect row(
            { startX, startY + i * rowHeight },
            { width, rowHeight }
        );

        if (row.contains(mousePos))
        {
            browser.selectedIndex = i;

            browser.selectedFile =
                browser.entries[i].path();

            break;
        }
    }
}