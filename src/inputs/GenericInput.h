#pragma once

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

// update editor cursor visibility function for blinking cursor in text editing fields
void updateCursor(
    EditorState& editor,
    AppState state,
    float dt)
{
    editor.cursorTimer += dt;

    if (editor.cursorTimer < 0.5f)
        return;

    editor.cursorVisible =
        !editor.cursorVisible;

    editor.cursorTimer = 0.f;
}