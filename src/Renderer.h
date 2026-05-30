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

// function to rebuild the colour picker gradients based on current hue, saturation and value
void rebuildColourPickerGradients(
    ColourPickerLayout& picker,
    float hue,
    float saturation,
    float value)
{
    picker.hueBar.clear();
    picker.saturationBar.clear();
    picker.valueBar.clear();

    const float sliderX =
        picker.sliderX();

    const float sliderY =
        picker.sliderY();

    const float hueY =
        sliderY;

    const float satY =
        sliderY +
        picker.height +
        picker.sliderSpacing;

    const float valueY =
        sliderY +
        (picker.height + picker.sliderSpacing) * 2.f;

    for (int i = 0; i <= static_cast<int>(picker.width); ++i)
    {
        float t =
            static_cast<float>(i) /
            picker.width;

        float x =
            sliderX +
            static_cast<float>(i);

        sf::Color hueColour =
            hsvToRgb(
                t * 360.f,
                1.f,
                1.f);

        picker.hueBar.append({
            { x, hueY },
            hueColour
            });

        picker.hueBar.append({
            { x, hueY + picker.height },
            hueColour
            });

        sf::Color satColour =
            hsvToRgb(
                hue,
                t,
                value);

        picker.saturationBar.append({
            { x, satY },
            satColour
            });

        picker.saturationBar.append({
            { x, satY + picker.height },
            satColour
            });

        sf::Color valueColour =
            hsvToRgb(
                hue,
                saturation,
                t);

        picker.valueBar.append({
            { x, valueY },
            valueColour
            });

        picker.valueBar.append({
            { x, valueY + picker.height },
            valueColour
            });
    }

    picker.cachedHue = static_cast<int>(hue);
    picker.cachedSaturation = static_cast<int>(saturation * 100.f);
    picker.cachedValue = static_cast<int>(value * 100.f);

    picker.gradientsDirty = false;
}

// draw wheel function
void drawWheel(sf::RenderWindow& window,
    const std::vector<Segment>& segments,
    float radius,
    sf::Vector2f center,
    float rotation,
    const sf::Font& font
)
{
    float totalWeight = 0.f;
    for (const auto& s : segments)
        totalWeight += s.weight;

    float currentAngle = rotation;

    for (const auto& s : segments)
    {
        float angle = (s.weight / totalWeight) * 360.f;

        // lock angles for this segment
        float segmentStart = currentAngle;
        float midAngle = segmentStart + angle * 0.5f;

        // draw slice
        sf::VertexArray slice(sf::PrimitiveType::TriangleFan);

        sf::Vertex centerVertex;
        centerVertex.position = center;
        centerVertex.color = s.colour;
        slice.append(centerVertex);

        int points = 128;

        for (int i = 0; i <= points; ++i)
        {
            float current = segmentStart + angle * (float(i) / points);
            float rad = current * PI / 180.f;

            sf::Vector2f point = {
                center.x + std::cos(rad) * radius,
                center.y + std::sin(rad) * radius
            };

            sf::Vertex v;
            v.position = point;
            v.color = s.colour;
            slice.append(v);
        }

        window.draw(slice);

        // draw labels
        float rad = midAngle * PI / 180.f;

        float textRadius = radius * 0.5f;        // place text in true visual center of segment

        sf::Vector2f textPos = {
            center.x + std::cos(rad) * textRadius,
            center.y + std::sin(rad) * textRadius
        };

        sf::Text text(font);
        text.setString(s.label);

        // base size of text relative to wheel size, will be scaled down if it doesn't fit
        unsigned int charSize = static_cast<unsigned int>(radius * 0.2f);
        text.setCharacterSize(charSize);

        // measure text bounds to check if it fits within the segment
        sf::FloatRect bounds = text.getLocalBounds();

        // available width along arc
        float arcLength = (angle * PI / 180.f) * textRadius;
        float maxWidth = arcLength * 0.75f;

        // Scale down if needed
        if (bounds.size.x > maxWidth)
        {
            float scale = maxWidth / bounds.size.x;
            text.setCharacterSize(static_cast<unsigned int>(charSize * scale));
        }

        // Recalculate bounds after resizing
        bounds = text.getLocalBounds();

        // Proper centering
        text.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });

        text.setPosition(textPos);

        // Affix text to the segment
        text.setRotation(sf::degrees(midAngle + 180.f));

        // Contrast-aware colour
        float brightness =
            0.299f * s.colour.r +
            0.587f * s.colour.g +
            0.114f * s.colour.b;

        text.setFillColor(brightness > 128.f ? sf::Color::Black : sf::Color::White);

        window.draw(text);

        // Advance to next segment
        currentAngle += angle;
    }
}

// draw pointer function
sf::Vector2f drawPointer(sf::RenderWindow& window,
    float radius,
    const sf::Vector2f& center)
{
    float height = radius * 0.12f;
    float width = radius * 0.08f;

    // tip offset into the wheel
    float overlap = radius * 0.05f;

    sf::Vector2f tip = {
        center.x,
        center.y - radius + overlap
    };

    sf::VertexArray pointer(sf::PrimitiveType::Triangles, 3);

    // Tip
    pointer[0].position = tip;

    // Base
    pointer[1].position = tip + sf::Vector2f(-width * 0.6f, -height);
    pointer[2].position = tip + sf::Vector2f(width * 0.6f, -height);

    for (int i = 0; i < 3; ++i)
        pointer[i].color = sf::Color::Black;

    window.draw(pointer);

    return tip;
}

// draw wheel name function
void drawWheelName(const sf::Font& font,
    const Wheel& wheel,
    sf::RenderWindow& window,
    const sf::Vector2f& center)
{
    sf::Text title(font);
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::Black);
    title.setString(wheel.name);

    // center horizontally
    sf::FloatRect bounds = title.getGlobalBounds();

    title.setPosition({ center.x - bounds.size.x * 0.5f, 20.f });

    window.draw(title);
}

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

// draw load wheel UI function
void drawLoadWheelUI(sf::RenderWindow& window,
    const sf::Font& font)
{
    sf::Text title(font);
    title.setString("Load Wheel");
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::Black);
    title.setPosition({ 50.f, 50.f });

    window.draw(title);

    sf::Text comingSoon(font);
    comingSoon.setString("Coming soon");
    comingSoon.setCharacterSize(22);
    comingSoon.setFillColor(sf::Color(80, 80, 80));

    comingSoon.setPosition({
        50.f,
        100.f
        });

    window.draw(comingSoon);
}

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

// draw editor title function
void drawEditorTitle(
    sf::RenderWindow& window,
    const sf::Font& font,
    const EditorState& editor,
    float startX,
    float startY,
    float tableWidth)
{
    // wheel name
    float boxWidth = tableWidth * 0.6f;
    float boxHeight = 32.f;

    // wheel name box position
    float boxX = startX + tableWidth - boxWidth;
    float boxY = startY - 50.f;

    // background box
    sf::RectangleShape titleBox({ boxWidth, boxHeight });
    titleBox.setPosition({ boxX, boxY });
    titleBox.setFillColor(sf::Color(255, 255, 255));
    titleBox.setOutlineThickness(2.f);
    titleBox.setOutlineColor(
        (editor.activeField == EditField::Title)
        ? sf::Color(100, 150, 250)
        : sf::Color(180, 180, 180)
    );
    window.draw(titleBox);

    // wheel title text
    std::string displayName =
        editor.wheelName.empty()
        ? "Untitled Wheel"
        : editor.wheelName;

    drawEditableText(
        window,
        font,
        displayName,
        { boxX + 6.f, boxY + 5.f },
        editor.activeField == EditField::Title,
        editor.textSelected,
        editor.cursorVisible,
        boxWidth - 12.f
    );
}

// draw editor headers function
void drawEditorHeaders(
    sf::RenderWindow& window,
    const sf::Font& font,
    float startX,
    float startY,
    float colLabel,
    float colColour)
{
    constexpr float columnGap = 8.f;
    
    // headers bold
    sf::Text header(font);
    header.setCharacterSize(20);
    header.setFillColor(sf::Color::Black);
    header.setStyle(sf::Text::Bold);

    header.setString("Label");
    header.setPosition({ startX, startY });
    window.draw(header);

    header.setString("Colour");
    header.setPosition({
        startX + colLabel + columnGap,
        startY
        });
    window.draw(header);

    header.setString("Weight");
    header.setPosition({
        startX +
        colLabel +
        columnGap +
        colColour +
        columnGap,
        startY
        });
    window.draw(header);
}

// draw editor rows function
void drawEditorRows(
    sf::RenderWindow& window,
    const sf::Font& font,
    const EditorState& editor,
    const TableLayout& layout,
    int startRow,
    int endRow)
{
    constexpr float textOffsetY = 5.f;
    constexpr float colourPadX = 8.f;
    constexpr float colourPadY = 5.f;

    for (int i = startRow; i < endRow; ++i)
    {
        const auto& segment = editor.segments[i];

        const bool selected =
            (i == editor.selectedRow);

        const int visibleIndex =
            i - startRow;

        const float y =
            layout.startY +
            (visibleIndex + 1) * layout.rowHeight;

        const RowCellRects rects =
            getRowCellRects(layout, y);

        sf::FloatRect labelContent =
            getCellContentRect(rects.label);

        sf::FloatRect colourContent =
            getCellContentRect(rects.colour);

        sf::FloatRect weightContent =
            getCellContentRect(rects.weight);

        const float rowTop =
            y - layout.rowPadding;

        const float rowHeight =
            layout.rowHeight +
            layout.rowPadding * 2.f;

        // row highlight

        if (selected)
        {
            sf::RectangleShape highlight({
                layout.getRowWidth(),
                rowHeight
                });

            highlight.setPosition({
                layout.startX,
                rowTop
                });

            highlight.setFillColor(
                sf::Color(200, 220, 255)
            );

            window.draw(highlight);
        }

        // active cell highlight

        auto drawCellHighlight =
            [&](const sf::FloatRect& rect)
            {
                sf::FloatRect content =
                    getCellContentRect(rect);

                sf::RectangleShape cell(content.size);

                cell.setPosition(content.position);

                cell.setFillColor(
                    sf::Color(160, 190, 245)
                );

                window.draw(cell);
            };

        if (selected)
        {
            switch (editor.activeField)
            {
            case EditField::Label:
                drawCellHighlight(rects.label);
                break;

            case EditField::Colour:
                drawCellHighlight(rects.colour);
                break;

            case EditField::Weight:
                drawCellHighlight(rects.weight);
                break;

            default:
                break;
            }
        }

        // label

        drawEditableText(
            window,
            font,
            segment.label,
            {
                labelContent.position.x + 5.f,
                labelContent.position.y + textOffsetY
            },
            selected &&
            editor.activeField == EditField::Label,
            editor.textSelected,
            editor.cursorVisible,
            labelContent.size.x
        );

        // colour box

        const float colourBoxWidth = colourContent.size.x - colourPadX * 2.f;

        const float colourBoxHeight = layout.rowHeight - 20.f;

        sf::RectangleShape colourBox({
            colourBoxWidth,
            colourBoxHeight
            });

        colourBox.setFillColor(segment.colour);

        colourBox.setPosition({
            colourContent.position.x + colourPadX,
            colourContent.position.y +
            (colourContent.size.y - colourBoxHeight) * 0.5f
            });

        window.draw(colourBox);

        // weight text

        std::string weightText;

        if (selected &&
            editor.activeField == EditField::Weight)
        {
            weightText =
                editor.weightInput;
        }
        else
        {
            std::ostringstream ss;

            ss << std::fixed
                << std::setprecision(2)
                << segment.weight;

            weightText = ss.str();
        }

        drawEditableText(
            window,
            font,
            weightText,
            {
                weightContent.position.x + 5.f,
                weightContent.position.y + textOffsetY
            },
            selected &&
            editor.activeField == EditField::Weight,
            editor.textSelected,
            editor.cursorVisible,
            weightContent.size.x
        );
    }
}

void drawColourPicker(
    sf::RenderWindow& window,
    ColourPickerLayout picker,
    const sf::Color& previewColour,
    const EditorState& editor)
{
    const float sliderX =
        picker.sliderX();

    const float sliderY =
        picker.sliderY();

    if (picker.gradientsDirty ||
        picker.cachedHue != editor.hue ||
        picker.cachedSaturation != editor.saturation ||
        picker.cachedValue != editor.value)
    {
        rebuildColourPickerGradients(
            picker,
            editor.hue,
            editor.saturation,
            editor.value);
    }

    sf::RectangleShape panel({
    picker.panelWidth(),
    picker.panelHeight()
        });

    panel.setPosition({ picker.x, picker.y });

    panel.setFillColor(sf::Color(240, 240, 240));

    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color::Black);

    window.draw(panel);

    window.draw(picker.hueBar);
    window.draw(picker.saturationBar);
    window.draw(picker.valueBar);

    sf::RectangleShape outline({ picker.width, picker.height });

    outline.setFillColor(sf::Color::Transparent);

    outline.setOutlineThickness(2.f);
    outline.setOutlineColor(sf::Color::Black);

    outline.setPosition({
    sliderX,
    sliderY
        });

    window.draw(outline);

    outline.setPosition({
    sliderX,
    sliderY +
    picker.height +
    picker.sliderSpacing
        });

    window.draw(outline);

    outline.setPosition({
    sliderX,
    sliderY +
    (picker.height +
     picker.sliderSpacing) * 2.f
        });

    window.draw(outline);

    sf::RectangleShape preview({
    picker.previewSize(),
    picker.previewSize()
        });

    preview.setFillColor(
        previewColour
    );

    preview.setPosition({
        sliderX +
        picker.width +
        picker.previewGap,

        sliderY
        });

    preview.setOutlineThickness(2.f);
    preview.setOutlineColor(sf::Color::Black);

    window.draw(preview);
}

// draw create wheel UI function
void drawCreateWheelUI(sf::RenderWindow& window,
    const sf::Font& font,
    const EditorState& editor,
    TableLayout& layout,
    const ColourPickerLayout& picker)
{
    // editor title
    sf::Text title(font);
    title.setString("Edit Wheel");
    title.setCharacterSize(30);
    title.setPosition({ layout.startX, layout.startY - 60.f });
    title.setFillColor(sf::Color::Black);
    window.draw(title);

    int maxRows = static_cast<int>((window.getSize().y - layout.startY - 140.f) / layout.rowHeight);
    int totalRows = static_cast<int>(editor.segments.size());
    int startRow = std::min(editor.scrollOffset, std::max(0, totalRows - maxRows));
    int endRow = std::min(startRow + maxRows, totalRows);

    float tableHeight = maxRows * layout.rowHeight;

    float scrollbarX =
        layout.startX +
        layout.getRowWidth() +
        10.f;

    float scrollbarY =
        layout.startY + layout.rowHeight;

    drawEditorTitle(
        window,
        font,
        editor,
        layout.startX,
        layout.startY,
        layout.tableWidth);

    drawEditorHeaders(
        window,
        font,
        layout.startX,
        layout.startY,
        layout.colLabel,
        layout.colColour
    );

    drawEditorRows(
        window,
        font,
        editor,
        layout,
        startRow,
        endRow
    );

    drawScrollbar(
        window,
        scrollbarX,
        scrollbarY,
        tableHeight,
        maxRows,
        totalRows,
        editor.scrollOffset
    );

    if (editor.showColourPicker &&
        editor.hasSelectedRow())
    {
        drawColourPicker(
            window,
            picker,
            editor.segments[editor.selectedRow].colour,
            editor);
    }

    // instruction footer
    sf::Text instructions(font);
    instructions.setCharacterSize(16);
    instructions.setFillColor(sf::Color::Black);
    instructions.setString("[A] Add Row     [R] Remove Row    [S] Save");
    instructions.setPosition({ layout.startX, window.getSize().y - 42.f });

    window.draw(instructions);
}