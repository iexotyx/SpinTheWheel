#pragma once

#include "TextEditor.h"
#include "util/Table.h"

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

void drawResetSegmentsButton(
    sf::RenderWindow& window,
    const Button& clearSegmentsButton)
{
    clearSegmentsButton.draw(window);
}

// draw create/edit wheel UI function
void drawEditWheelUI(sf::RenderWindow& window,
    const sf::Font& font,
    const EditorState& editor,
    TableLayout& layout,
    const ColourPickerLayout& picker,
    Button& resetSegmentsButton)
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

    updateEditorButtonPos(
        layout,
        tableHeight,
        resetSegmentsButton
    );

    drawResetSegmentsButton(
        window,
        resetSegmentsButton
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
    instructions.setString("[A] Add Row     [R] Remove Row    [S] Save     [CTRL + S] Save to File");
    instructions.setPosition({ layout.startX, window.getSize().y - 42.f });

    window.draw(instructions);
}