#pragma once

// class for handling keyboard shortcuts
enum class Shortcut
{
    AddSegment,
    RemoveSegment,
    SaveWheel,
    SpinWheel,
    CancelEdit
};

// function to check if a given key press matches a specific shortcut
inline bool shortcutPressed(
    const sf::Event::KeyPressed& key,
    Shortcut shortcut)
{
    switch (shortcut)
    {
    case Shortcut::AddSegment:
        return key.code == sf::Keyboard::Key::A;

    case Shortcut::RemoveSegment:
        return key.code == sf::Keyboard::Key::R;

    case Shortcut::SaveWheel:
        return key.code == sf::Keyboard::Key::S;

    case Shortcut::SpinWheel:
        return key.code == sf::Keyboard::Key::Space;

    case Shortcut::CancelEdit:
        return key.code == sf::Keyboard::Key::Escape;
    }

    return false;
}

// function to add a new segment to the editor state
inline void shortcutAddSegment(
    EditorState& editor)
{
    constexpr float HUE_STEP = 45.f;

    float baseHue = editor.hue;

    if (!editor.segments.empty())
    {
        float lastHue;
        float sat;
        float val;

        rgbToHsv(
            editor.segments.back().colour,
            lastHue,
            sat,
            val
        );

        baseHue = lastHue;
    }

    float newHue =
        std::fmod(baseHue + HUE_STEP, 360.f);

    editor.hue = newHue;
    editor.saturation = 1.f;
    editor.value = 1.f;

    sf::Color col =
        hsvToRgb(
            editor.hue,
            editor.saturation,
            editor.value
        );

    editor.segments.push_back(
        {
            "New",
            1.f,
            col
        });
}

// function to remove the last segment from the editor state
inline void shortcutRemoveSegment(
    EditorState& editor)
{
    if (!editor.hasSelectedRow())
        return;

    const int removedRow =
        editor.selectedRow;

    editor.segments.erase(
        editor.segments.begin() + removedRow
    );

    if (editor.segments.empty())
    {
        editor.selectedRow = -1;
        editor.showColourPicker = false;
        return;
    }

    // Select the row that moved into this slot.
    if (removedRow >= static_cast<int>(editor.segments.size()))
    {
        editor.selectedRow =
            static_cast<int>(editor.segments.size()) - 1;
    }
    else
    {
        editor.selectedRow = removedRow;
    }
}

// function to save the current wheel from the editor state to the main app state
inline void shortcutSaveWheel(
    EditorState& editor,
    Wheel& wheel,
    AppState& state)
{
    normalizeWeights(editor.segments);

    wheel.segments =
        editor.segments;

    wheel.name =
        editor.wheelName.empty()
        ? "Untitled Wheel"
        : editor.wheelName;

    state = AppState::MainView;
}

// function to initialize the wheel spin state with random values to start spinning the wheel
inline void shortcutSpinWheel(
    WheelSpinState& spin)
{
    if (spin.spinning)
        return;

    spin.rotation =
        static_cast<float>(
            std::rand() % 2160);

    spin.angularVelocity =
        720.f +
        static_cast<float>(
            std::rand() % 180);

    spin.deceleration =
        0.02f +
        static_cast<float>(
            std::rand()) /
        RAND_MAX * 0.02f;

    spin.spinning = true;
}

// function to cancel weight editing and reset the weight input field to the current segment weight
inline void shortcutCancelWeightEdit(
    EditorState& editor,
    std::ostringstream& ss)
{
    if (editor.activeField != EditField::Weight)
        return;

    if (!editor.hasSelectedRow())
        return;

    ss.str("");
    ss.clear();

    ss << editor.segments[
        editor.selectedRow
    ].weight;

    editor.weightInput =
        ss.str();

    editor.textSelected = false;
}