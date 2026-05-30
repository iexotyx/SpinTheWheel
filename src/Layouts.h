#pragma once

#include "util/ColourUtils.h"
#include "util/Button.h"
#include "util/Scrollbar.h"

// table layout struct to hold calculated positions and sizes for the create/edit wheel UI
struct TableLayout
{
    float startX;
    float startY;

    float colLabel;
    float colColour;
    float colWeight;

    float rowHeight;
    float tableWidth;

    float rowPadding = 4.f;
    float columnGap = 8.f;

    float getRowWidth() const
    {
        return
            colLabel +
            colColour +
            colWeight +
            columnGap * 2.f;
    }
};

// struct to hold the rectangles for each cell in a row
struct RowCellRects
{
    sf::FloatRect label;
    sf::FloatRect colour;
    sf::FloatRect weight;
};

// function to get the rectangles for each cell in a given row
inline RowCellRects getRowCellRects(
    const TableLayout& layout,
    float y)
{

    float colourX =
        layout.startX +
        layout.colLabel +
        layout.columnGap;

    float weightX =
        colourX +
        layout.colColour +
        layout.columnGap;

    return
    {
        {{layout.startX,  y}, {layout.colLabel,  layout.rowHeight}},
        {{colourX, y}, {layout.colColour, layout.rowHeight}},
        {{weightX, y}, {layout.colWeight, layout.rowHeight}}
    };
}

// function to get the inner content rectangle of a cell
inline sf::FloatRect getCellContentRect(
    const sf::FloatRect& rect,
    float padding = 4.f)
{
    return {
        {
            rect.position.x + padding,
            rect.position.y + padding
        },
        {
            rect.size.x - padding * 2.f,
            rect.size.y - padding * 2.f
        }
    };
}

// colour picker layout struct to hold positions and sizes for the colour picker UI
struct ColourPickerLayout
{
    float x;
    float y;

    float width;
    float height;

    float panelPadding = 12.f;
    float sliderSpacing = 8.f;
    float previewGap = 12.f;

    sf::VertexArray hueBar{ sf::PrimitiveType::TriangleStrip };
    sf::VertexArray saturationBar{ sf::PrimitiveType::TriangleStrip };
    sf::VertexArray valueBar{ sf::PrimitiveType::TriangleStrip };

    int cachedHue = -1;
    int cachedSaturation = -1;
    int cachedValue = -1;

    bool gradientsDirty = true;

    float stackHeight() const
    {
        return height * 3.f +
            sliderSpacing * 2.f;
    }

    float previewSize() const
    {
        return stackHeight();
    }

    float panelWidth() const
    {
        return width +
            previewGap +
            previewSize() +
            panelPadding * 2.f;
    }

    float panelHeight() const
    {
        return stackHeight() +
            panelPadding * 2.f;
    }

    float sliderX() const
    {
        return x + panelPadding;
    }

    float sliderY() const
    {
        return y + panelPadding;
    }
};

// function to calculate table layout based on current window size
TableLayout getTableLayout(const sf::RenderWindow& window)
{
    sf::Vector2u size = window.getSize();

    TableLayout layout;
    layout.startX = size.x * 0.1f;
    layout.startY = size.y * 0.15f;
    layout.colLabel = size.x * 0.3f;
    layout.colColour = size.x * 0.2f;
    layout.colWeight = size.x * 0.2f;
    layout.rowHeight = 40.f;
    layout.tableWidth = layout.getRowWidth();

    return layout;
}

// function to calculate colour picker layout based on table layout and current row count
ColourPickerLayout getColourPickerLayout(
    int rowCount,
    const TableLayout& cachedLayout
)
{
    TableLayout layout = cachedLayout;

    ColourPickerLayout picker;

    picker.x = layout.startX;

    // position directly below the table based on row count
    picker.y = layout.startY + layout.rowHeight * (rowCount + 1) + 20.f;

    picker.width = 300.f;
    picker.height = 20.f;

    return picker;
}