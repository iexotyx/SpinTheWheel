#pragma once

// edit field types and variables for create/edit wheel UI
enum class EditField
{
    None,
    Label,
    Weight,
    Colour,
    Title
};

// text edit result for handling input in the create/edit wheel UI
enum class TextEditResult
{
    None,
    Commit
};

// editor state struct to hold all relevant data for the create/edit wheel UI
struct EditorState
{
    std::vector<Segment> segments;
    std::string wheelName;

    int selectedRow = -1;
    int scrollOffset = 0;

    EditField activeField = EditField::None;

    std::string weightInput;

    bool textSelected = false;

    bool showColourPicker = false;

    float hue = 0.f;
    float saturation = 1.f;
    float value = 1.f;

    bool draggingHue = false;
    bool draggingSaturation = false;
    bool draggingValue = false;

    sf::Clock clickClock;
    float lastClickTime = 0.f;

    bool cursorVisible = true;
    float cursorTimer = 0.f;

    // utility function to reset the editor state when creating a new wheel or cancelling edits
    void resetEditorState()
    {
        selectedRow = -1;
        activeField = EditField::None;
        weightInput.clear();
        textSelected = false;
        showColourPicker = false;

        hue = 0.f;
        saturation = 1.f;
        value = 1.f;
    };

    // commit weight edit function
    void commitWeightEdit()
    {
        if (selectedRow < 0 ||
            selectedRow >= segments.size())
        {
            return;
        }
        try
        {
            float value =
                std::stof(weightInput);

            if (value > 0.f)
            {
                segments[selectedRow].weight = value;
            }
        }
        catch (...)
        {
            std::ostringstream ss;

            ss << segments[selectedRow].weight;

            weightInput = ss.str();
        }

        textSelected = false;

    };

    // utility function to check if a valid segment row is selected in the editor state for enabling edit/delete actions
    bool hasSelectedRow() const
    {
        return selectedRow >= 0 &&
            selectedRow < (segments.size());
    }
};

// weight normalisation function to ensure segments are drawn proportionally
void normalizeWeights(std::vector<Segment>& segments)
{
    float total = 0.f;

    for (const auto& s : segments)
        total += s.weight;

    if (total <= 0.f)
        return;

    for (auto& s : segments)
        s.weight /= total;
}