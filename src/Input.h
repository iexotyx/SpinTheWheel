#pragma once

// handle text input function
TextEditResult handleTextInput(
    const sf::Event::TextEntered& textEvent,
    std::string& text,
    bool& textSelected
)
{
    char entered =
        static_cast<char>(textEvent.unicode);

    // enter commits

    if (entered == 13 || entered == 10)
    {
        textSelected = false;
        return TextEditResult::Commit;
    }

    // backspace

    if (entered == 8 || entered == 7)
    {
        if (textSelected)
        {
            text.clear();
            textSelected = false;
        }
        else if (!text.empty())
        {
            text.pop_back();
        }

        return TextEditResult::None;
    }

    // printable ascii

    if (entered >= 32 && entered < 127)
    {
        if (textSelected)
        {
            text.clear();
            textSelected = false;
        }

        text += entered;
    }

    return TextEditResult::None;
}

// handle numeric text input function, allows digits and one decimal point only
TextEditResult handleNumericTextInput(
    const sf::Event::TextEntered& textEvent,
    std::string& text,
    bool& textSelected
)
{
    char entered =
        static_cast<char>(textEvent.unicode);

    if (entered == 13 || entered == 10)
    {
        textSelected = false;
        return TextEditResult::Commit;
    }

    if (entered == 8)
    {
        if (textSelected)
        {
            text.clear();
            textSelected = false;
        }
        else if (!text.empty())
        {
            text.pop_back();
        }

        return TextEditResult::None;
    }

    if ((entered >= '0' && entered <= '9') ||
        entered == '.')
    {
        if (textSelected)
        {
            text.clear();
            textSelected = false;
        }

        if (entered != '.' ||
            text.find('.') == std::string::npos)
        {
            text += entered;
        }
    }

    return TextEditResult::None;
}

// handle mouse click in editor function
void handleEditorMouseClick(
    const sf::Vector2f& mousePos,
    sf::RenderWindow& window,
    const TableLayout& layout,
    const ColourPickerLayout& picker,
    EditorState& editor,
    sf::Clock& clickClock,
    float doubleClickThreshold)
{
    bool clickedSomething = false;

    // title box

    float boxWidth =
        layout.tableWidth * 0.6f;

    float boxHeight = 32.f;

    float boxX =
        layout.startX +
        layout.tableWidth -
        boxWidth;

    float boxY =
        layout.startY - 50.f;

    sf::FloatRect titleRect(
        { boxX, boxY },
        { boxWidth, boxHeight }
    );

    editor.commitWeightEdit();

    int maxRows = static_cast<int>(
        (window.getSize().y -
            layout.startY -
            140.f) /
        layout.rowHeight
        );

    int totalRows =
        static_cast<int>(
            editor.segments.size());

    editor.scrollOffset =
        std::min(
            editor.scrollOffset,
            std::max(
                0,
                totalRows - maxRows));

    int startRow =
        editor.scrollOffset;

    int endRow =
        std::min(
            startRow + maxRows,
            totalRows);

    float now =
        clickClock
        .getElapsedTime()
        .asSeconds();

    bool isDoubleClick =
        (now - editor.lastClickTime)
        < doubleClickThreshold;

    // title

    if (titleRect.contains(mousePos))
    {
        editor.selectedRow = -1;
        editor.activeField = EditField::Title;
        editor.textSelected = isDoubleClick;
        editor.showColourPicker = false;
        editor.lastClickTime = now;

        clickedSomething = true;
    }

    // rows

    for (int i = startRow;
        !clickedSomething && i < endRow;
        ++i)
    {
        float y =
            layout.startY +
            ((i - startRow) + 1) *
            layout.rowHeight;

        auto rects =
            getRowCellRects(
                layout,
                y);

        EditField field =
            EditField::None;

        if (rects.label.contains(mousePos))
            field = EditField::Label;
        else if (rects.colour.contains(mousePos))
            field = EditField::Colour;
        else if (rects.weight.contains(mousePos))
            field = EditField::Weight;

        // row padding click

        if (field == EditField::None)
        {
            sf::FloatRect rowRect(
                {
                    layout.startX,
                    y - layout.rowPadding
                },
                {
                    layout.tableWidth,
                    layout.rowHeight +
                    layout.rowPadding * 2.f
                });

                if (rowRect.contains(mousePos))
                {
                    editor.selectedRow = i;
                    editor.activeField = EditField::None;
                    editor.showColourPicker = false;

                    clickedSomething = true;
                    break;
                }

                continue;
        }

        editor.selectedRow = i;
        editor.activeField = field;
        editor.lastClickTime = now;

        clickedSomething = true;

        switch (field)
        {
        case EditField::Label:
            editor.showColourPicker = false;
            editor.textSelected = isDoubleClick;
            break;

        case EditField::Colour:
            editor.showColourPicker = true;
            editor.textSelected = false;

            rgbToHsv(
                editor.segments[i].colour,
                editor.hue,
                editor.saturation,
                editor.value);

            break;

        case EditField::Weight:
        {
            std::ostringstream ss;

            ss << std::fixed
                << std::setprecision(6)
                << editor.segments[i].weight;

            editor.weightInput =
                ss.str();

            editor.showColourPicker = false;
            editor.textSelected = true;

            break;
        }

        default:
            break;
        }
    }

    // keep selection visible

    if (editor.hasSelectedRow())
    {
        int visibleRows =
            static_cast<int>(
                (window.getSize().y -
                    layout.startY -
                    140.f) /
                layout.rowHeight);

        if (editor.selectedRow <
            editor.scrollOffset)
        {
            editor.scrollOffset =
                editor.selectedRow;
        }

        if (editor.selectedRow >=
            editor.scrollOffset +
            visibleRows)
        {
            editor.scrollOffset =
                editor.selectedRow -
                visibleRows +
                1;
        }
    }

    // picker

    if (editor.showColourPicker)
    {
        const float sliderX =
            picker.sliderX();

        const float sliderY =
            picker.sliderY();

        const float h =
            picker.height;

        sf::FloatRect hueRect(
        {
            sliderX,
            sliderY
        },
        {
            picker.width,
            h
        });

        sf::FloatRect satRect(
            {
                sliderX,
                sliderY +
                h +
                picker.sliderSpacing
            },
        {
            picker.width,
            h
        });

        sf::FloatRect valRect(
            {
                sliderX,
                sliderY +
                (h + picker.sliderSpacing) * 2.f
            },
        {
            picker.width,
            h
        });

            if (hueRect.contains(mousePos))
            {
                editor.draggingHue = true;
                clickedSomething = true;
            }
            else if (satRect.contains(mousePos))
            {
                editor.draggingSaturation = true;
                clickedSomething = true;
            }
            else if (valRect.contains(mousePos))
            {
                editor.draggingValue = true;
                clickedSomething = true;
            }
    }

    // clear selection

    if (!clickedSomething)
    {
        editor.selectedRow = -1;
        editor.activeField = EditField::None;
        editor.showColourPicker = false;
    }
}

// handle text input in editor function
void handleEditorTextInput(
    const sf::Event::TextEntered& textEvent,
    EditorState& editor
)
{
    if (editor.activeField == EditField::None)
        return;

    // title
    if (editor.activeField == EditField::Title)
    {
        handleTextInput(
            textEvent,
            editor.wheelName,
            editor.textSelected
        );

        return;
    }

    if (editor.selectedRow < 0 ||
        editor.selectedRow >= editor.segments.size())
    {
        return;
    }

    // label
    if (editor.activeField == EditField::Label)
    {
        handleTextInput(
            textEvent,
            editor.segments[
                editor.selectedRow
            ].label,
            editor.textSelected
                    );

        return;
    }

    // weight
    if (editor.activeField == EditField::Weight)
    {
        TextEditResult result =
            handleNumericTextInput(
                textEvent,
                editor.weightInput,
                editor.textSelected
            );

        if (result == TextEditResult::Commit)
        {
            editor.commitWeightEdit();
        }

        return;
    }
}

// handle scrolling in editor function
void handleEditorScroll(
    float scrollDelta,
    const sf::RenderWindow& window,
    const TableLayout& layout,
    EditorState& editor
)
{
    int visibleRows =
        static_cast<int>(
            (window.getSize().y
                - layout.startY
                - 140.f)
            / layout.rowHeight
            );

    handleScrollInput(
        scrollDelta,
        editor.scrollOffset,
        static_cast<int>(
            editor.segments.size()
            ),
        visibleRows
    );
}