#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include "Segment.h"

// menu button types
enum class ButtonType
{
    Text,
    MenuIcon
};

// edit field types and variables for create/edit wheel UI
enum class EditField
{
    None,
    Label,
    Weight,
    Colour
};

struct TableLayout
{
    float startX;
    float startY;
    float colLabel;
    float colcolour;
    float colWeight;
    float rowHeight;
};

TableLayout getTableLayout(const sf::RenderWindow& window)
{
    sf::Vector2u size = window.getSize();

    TableLayout layout;
    layout.startX = size.x * 0.1f;
    layout.startY = size.y * 0.15f;
    layout.colLabel = size.x * 0.3f;
    layout.colcolour = size.x * 0.2f;
    layout.colWeight = size.x * 0.2f;
    layout.rowHeight = 40.f;

    return layout;
}

// colour picker setup
float hue = 0.f;
float saturation = 1.f;
float value = 1.f;

bool showColourPicker = false;

struct ColourPickerLayout
{
    float x;
    float y;
    float width;
    float height;
};

ColourPickerLayout getColourPickerLayout(sf::RenderWindow& window, int rowCount, TableLayout cachedLayout)
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

sf::Color hsvToRgb(float h, float s, float v)
{
    float c = v * s;
    float x = c * (1 - std::abs(fmod(h / 60.f, 2) - 1));
    float m = v - c;

    float r = 0, g = 0, b = 0;

    if (h < 60) { r = c; g = x; }
    else if (h < 120) { r = x; g = c; }
    else if (h < 180) { g = c; b = x; }
    else if (h < 240) { g = x; b = c; }
    else if (h < 300) { r = x; b = c; }
    else { r = c; b = x; }

    return sf::Color(
        static_cast<uint8_t>((r + m) * 255),
        static_cast<uint8_t>((g + m) * 255),
        static_cast<uint8_t>((b + m) * 255)
    );
}

void rgbToHsv(const sf::Color& colour, float& h, float& s, float& v)
{
    float r = colour.r / 255.f;
    float g = colour.g / 255.f;
    float b = colour.b / 255.f;

    float max = std::max({ r, g, b });
    float min = std::min({ r, g, b });
    float delta = max - min;

    v = max;

    if (delta < 0.00001f)
    {
        h = 0;
        s = 0;
        return;
    }

    s = delta / max;

    if (max == r)
        h = 60.f * fmod(((g - b) / delta), 6.f);
    else if (max == g)
        h = 60.f * (((b - r) / delta) + 2.f);
    else
        h = 60.f * (((r - g) / delta) + 4.f);

    if (h < 0) h += 360.f;
}

// button struct for menu options
struct Button
{
    ButtonType type;

    sf::RectangleShape background;          // common background for all buttons
    sf::Text text;                          // used for text buttons
    std::vector<sf::RectangleShape> bars;   // used for menu icon

    // constructor for both text buttons and menu icon button
    Button(ButtonType t,
        const sf::Font& font,
        const std::string& label,
        sf::Vector2f size,
        sf::Vector2f position)
        : type(t),
        text(font, label, 18)
    {
        // common setup for all buttons
        background.setSize(size);
        background.setPosition(position);
        background.setFillColor(sf::Color(50, 50, 50));

        if (type == ButtonType::Text)
        {
            text.setPosition(position + sf::Vector2f(10.f, 8.f));   // small padding for text
        }
        else if (type == ButtonType::MenuIcon)
        {
            float barWidth = size.x * 0.6f; // bars are 60% of button width
            float barHeight = 3.f;          // bar height is fixed
            float spacing = 8.f;            // space between bars

            // create horizontal bars for menu icon
            for (int i = 0; i < 3; ++i)
            {
                sf::RectangleShape bar;
                bar.setSize({ barWidth, barHeight });
                bar.setFillColor(sf::Color::White);

                bar.setPosition(position + sf::Vector2f(
                    size.x * 0.2f,
                    size.y * 0.3f + i * spacing
                ));

                bars.push_back(bar);
            }
        }
    }

    bool isHovered(sf::Vector2f mousePos) const
    {
        return background.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window) const
    {
        window.draw(background);

        if (type == ButtonType::Text)
        {
            window.draw(text);
        }
        else if (type == ButtonType::MenuIcon)
        {
            for (const auto& bar : bars)
                window.draw(bar);
        }
    }

    void updateHover(sf::Vector2f mousePos)
    {
        if (isHovered(mousePos))
            background.setFillColor(sf::Color(80, 80, 80));
        else
            background.setFillColor(sf::Color(50, 50, 50));
    }
};

void resetEditorState(
    int& selectedRow,
    EditField& activeField,
    std::string& weightInput,
    bool& textSelected,
    bool& showColourPicker,
    float& hue,
    float& saturation,
    float& value
)
{
    selectedRow = -1;
    activeField = EditField::None;
    weightInput.clear();
    textSelected = false;
    showColourPicker = false;

    hue = 0.f;
    saturation = 1.f;
    value = 1.f;
}

// function used to ensure new segment hues are not too close to existing ones for better visual distinction, with a default minimum gap
bool isHueTooClose(float newHue, const std::vector<Segment>& segments, float minGap = 25.f)
{
    for (const auto& s : segments)
    {
        float existingHue, sat, val;
        rgbToHsv(s.colour, existingHue, sat, val);

        float diff = std::abs(newHue - existingHue);

        // wrap-around handling
        diff = std::min(diff, 360.f - diff);

        if (diff < minGap)
            return true;
    }
    return false;
}

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

constexpr float PI = 3.14159265f;

// Segment ID based on position of the pointer -----------------------------------------------------------------
int getSelectedSegment(const std::vector<Segment>& segments,
                       sf::Vector2f center,
                       sf::Vector2f pointerTip,
                       float rotation)
{
    if (segments.empty())
        return -1;

    // Pointer angle in world space
    float dx = pointerTip.x - center.x;
    float dy = pointerTip.y - center.y;

    float pointerAngle = std::atan2(dy, dx) * 180.f / PI;

    if (pointerAngle < 0)
        pointerAngle += 360.f;

    // Total weight
    float totalWeight = 0.f;
    for (const auto& s : segments)
        totalWeight += s.weight;

    // walk segments
    float currentAngle = rotation; // match draw start

    for (int i = 0; i < segments.size(); ++i)
    {
        float segmentAngle = (segments[i].weight / totalWeight) * 360.f;

        float start = std::fmod(currentAngle, 360.f);
        if (start < 0) start += 360.f;

        float end = std::fmod(currentAngle + segmentAngle, 360.f);
        if (end < 0) end += 360.f;

        // handle wrap-around
        if (start < end)
        {
            if (pointerAngle >= start && pointerAngle < end)
                return i;
        }
        else
        {
            // wrapped segment
            if (pointerAngle >= start || pointerAngle < end)
                return i;
        }

        currentAngle += segmentAngle;
    }

    return segments.size() - 1; // fallback
}

// draw wheel function ----------------------------------------------------------------------------------------
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

        int points = 32;

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

// draw pointer function --------------------------------------------------------------------------------------
sf::Vector2f drawPointer(sf::RenderWindow& window,
    float radius,
    sf::Vector2f center)
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

// draw menu button function -----------------------------------------------------------------------------------
void drawMenuButton(sf::RenderWindow& window, const Button& button)
{
    button.draw(window);
}

// draw menu function ------------------------------------------------------------------------------------------
void drawMenu(sf::RenderWindow& window,
    const std::vector<Button>& options,
    const sf::Font& font)
{
    for (const auto& btn : options)
        btn.draw(window);
}

// draw create wheel UI function -------------------------------------------------------------------------------
void drawCreateWheelUI(sf::RenderWindow& window,
    const std::vector<Segment>& segments,
    const std::string& name,
    const sf::Font& font,
    int selectedRow,
    EditField activeField,
    const std::string& weightInput,
    bool cursorVisible,
    bool textSelected,
    const TableLayout& layout,
    const ColourPickerLayout& picker)
{
    float startX = layout.startX;
    float startY = layout.startY;
    float colLabel = layout.colLabel;
    float colcolour = layout.colcolour;
    float colWeight = layout.colWeight;
    float rowHeight = layout.rowHeight;

    float tableWidth = colLabel + colcolour + colWeight;

    // title
    sf::Text title(font);
    title.setString("Edit Wheel");
    title.setCharacterSize(30);
    title.setPosition({ startX, startY - 60.f });
    title.setFillColor(sf::Color::Black);
    window.draw(title);

    // headers bold
    sf::Text header(font);
    header.setCharacterSize(20);
    header.setFillColor(sf::Color::Black);
    header.setStyle(sf::Text::Bold);

    header.setString("Label");
    header.setPosition({ startX, startY });
    window.draw(header);

    header.setString("Colour");
    header.setPosition({ startX + colLabel, startY });
    window.draw(header);

    header.setString("Weight");
    header.setPosition({ startX + colLabel + colcolour, startY });
    window.draw(header);

    // rows
    int maxRows = static_cast<int>(
        (window.getSize().y - startY - 140.f) / rowHeight
        );

    int endRow = std::min<int>(segments.size(), std::max(0, maxRows));

    for (int i = 0; i < endRow; ++i)
    {
        const auto& s = segments[i];

        float y = startY + (i + 1) * rowHeight;

        // row highlight
        if (i == selectedRow)
        {
            sf::RectangleShape highlight({
                tableWidth,
				rowHeight - 6.f // vertical alignment to centre of row
                });

            highlight.setPosition({
                startX,
				y + 3.f // vertical alignment to centre of row
                });

            highlight.setFillColor(sf::Color(200, 220, 255));
            window.draw(highlight);
        }

        // cell highlight helper
        auto drawCellHighlight = [&](float x, float width)
            {
                sf::RectangleShape cell({
                    width,
                    rowHeight - 6.f // vertical alignment to centre of row
                    });

                cell.setPosition({
                    x,
                    y + 3.f // vertical alignment to centre of row
                    });

                cell.setFillColor(sf::Color(160, 190, 245)); // slightly darker for active cell
                window.draw(cell);
            };

        // label cell highlight
        if (i == selectedRow && activeField == EditField::Label)
        {
            drawCellHighlight(startX, colLabel);
        }

        // label
        sf::Text label(font);
        label.setString(s.label);
        label.setCharacterSize(18);
        label.setFillColor(sf::Color::Black);
        label.setPosition({ startX, y + 5.f });

        // draw selection highlight
        if (i == selectedRow && activeField == EditField::Label && textSelected)
        {
            sf::FloatRect bounds = label.getGlobalBounds();

            sf::RectangleShape selection({
                bounds.size.x + 6.f,
                bounds.size.y + 4.f
                });

            selection.setPosition({
                bounds.position.x - 3.f,
                bounds.position.y - 2.f
                });

            // greyscale highlight for contrast
            selection.setFillColor(sf::Color(180, 180, 180, 180));
            window.draw(selection);
        }

        window.draw(label);

        // label cursor
        if (i == selectedRow && activeField == EditField::Label && cursorVisible && !textSelected)
        {
            sf::FloatRect bounds = label.getGlobalBounds();

            sf::RectangleShape cursor({ 1.f, bounds.size.y });
            cursor.setFillColor(sf::Color::Black);

            float cursorX = bounds.position.x;
            if (!s.label.empty())
                cursorX += bounds.size.x;

            cursor.setPosition({ cursorX + 2.f, bounds.position.y });
            window.draw(cursor);
        }

        // colour cell highlight
        if (i == selectedRow && activeField == EditField::Colour)
        {
            drawCellHighlight(startX + colLabel, colcolour);
        }

        // colour box
        float paddingX = 8.f;
        float paddingY = 5.f;

        float boxWidth = colcolour - paddingX * 2.f;
        float boxHeight = rowHeight - paddingY * 2.f;

        sf::RectangleShape colourBox({ boxWidth, boxHeight });
        colourBox.setFillColor(s.colour);

        colourBox.setPosition({
            startX + colLabel + paddingX,
            y + paddingY
            });

        window.draw(colourBox);

        // weight cell highlight
        if (i == selectedRow && activeField == EditField::Weight)
        {
            drawCellHighlight(startX + colLabel + colcolour, colWeight);
        }

        // weight string logic
        std::string weightStr;

        if (i == selectedRow && activeField == EditField::Weight)
        {
            weightStr = weightInput; // show raw user input when editing
        }
        else
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << s.weight; // always 2dp when not editing
            weightStr = ss.str();
        }

        // weight text
        sf::Text weight(font);
        weight.setString(weightStr);
        weight.setCharacterSize(18);
        weight.setFillColor(sf::Color::Black);
        weight.setPosition({ startX + colLabel + colcolour, y + 5.f });

        // draw selection highlight behind text
        if (i == selectedRow && activeField == EditField::Weight && textSelected)
        {
            sf::FloatRect bounds = weight.getGlobalBounds();

            sf::RectangleShape selection({
                bounds.size.x + 6.f,
                bounds.size.y + 4.f
                });

            selection.setPosition({
                bounds.position.x - 3.f,
                bounds.position.y - 2.f
                });

            // greyscale highlight for contrast
            selection.setFillColor(sf::Color(180, 180, 180, 180));
            window.draw(selection);
        }

        window.draw(weight);

        // weight cursor
        if (i == selectedRow && activeField == EditField::Weight && cursorVisible && !textSelected)
        {
            sf::FloatRect bounds = weight.getGlobalBounds();

            sf::RectangleShape cursor({ 2.f, bounds.size.y });
            cursor.setFillColor(sf::Color::Black);

            float cursorX = bounds.position.x;
            if (!weightStr.empty())
                cursorX += bounds.size.x;

            cursor.setPosition({ cursorX + 2.f, bounds.position.y });
            window.draw(cursor);
        }
    }

    // colour picker
    if (showColourPicker && selectedRow >= 0 && selectedRow < segments.size())
    {
        float x = picker.x;
        float y = picker.y;

        float w = picker.width;
        float h = picker.height;
        float spacing = 10.f;

        sf::RectangleShape panel({ w + 120.f, 120.f });
        panel.setPosition({ x, y });
        panel.setFillColor(sf::Color(240, 240, 240));
        panel.setOutlineThickness(2.f);
        panel.setOutlineColor(sf::Color::Black);
        window.draw(panel);

        for (int i = 0; i < (int)w; ++i)
        {
            float t = (float)i / w;
            float hVal = t * 360.f;

            sf::Color col = hsvToRgb(hVal, 1.f, 1.f);

            sf::RectangleShape line({ 1.f, h });
            line.setPosition({ x + 10.f + i, y + 10.f });
            line.setFillColor(col);
            window.draw(line);
        }

        for (int i = 0; i < (int)w; ++i)
        {
            float t = (float)i / w;
            sf::Color col = hsvToRgb(hue, t, value);

            sf::RectangleShape line({ 1.f, h });
            line.setPosition({ x + 10.f + i, y + 10.f + h + spacing });
            line.setFillColor(col);
            window.draw(line);
        }

        for (int i = 0; i < (int)w; ++i)
        {
            float t = (float)i / w;
            sf::Color col = hsvToRgb(hue, saturation, t);

            sf::RectangleShape line({ 1.f, h });
            line.setPosition({ x + 10.f + i, y + 10.f + (h + spacing) * 2 });
            line.setFillColor(col);
            window.draw(line);
        }

        sf::RectangleShape outline({ w, h });
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineThickness(2.f);
        outline.setOutlineColor(sf::Color::Black);

        outline.setPosition({ x + 10.f, y + 10.f });
        window.draw(outline);

        outline.setPosition({ x + 10.f, y + 10.f + h + spacing });
        window.draw(outline);

        outline.setPosition({ x + 10.f, y + 10.f + (h + spacing) * 2 });
        window.draw(outline);

		// colour preview box
        sf::RectangleShape preview({ 60.f, 60.f });
        preview.setFillColor(segments[selectedRow].colour);
        preview.setPosition({ x + w + 30.f, y + 10.f });
        preview.setOutlineThickness(2.f);
        preview.setOutlineColor(sf::Color::Black);
        window.draw(preview);
    }

    // instruction footer
    sf::Text instructions(font);
    instructions.setCharacterSize(16);
    instructions.setFillColor(sf::Color::Black);
    instructions.setString("[A] Add Row     [R] Remove Row    [S] Save");
    instructions.setPosition({ startX, window.getSize().y - 42.f});

    window.draw(instructions);
}

// draw load wheel UI function -----------------------------------------------------------------------------------
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

// main function ------------------------------------------------------------------------------------------------
int main()
{
    // load font
    sf::Font font;

    // debug font loading
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        return 1;
    }

    enum class AppState
    {
        MainView,      // wheel
        MenuOpen,      // dropdown visible
        CreateWheel,   // editor screen
        LoadWheel      // file selection screen
    };

	AppState state = AppState::MainView;    // start in main view

	// wheel data structure
    struct Wheel
    {
        std::string name;
        std::vector<Segment> segments;
    };

    Wheel wheel;

	// initial wheel
    wheel.name = "Basic Wheel";

    wheel.segments = {
        {"Red", 1, sf::Color::Red},
        {"Green", 1, sf::Color::Green},
        {"Blue", 1, sf::Color::Blue}
    };

    // seed random (repeats every time the wheel is spun)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::vector<Segment> editingSegments;
    std::string editingWheelName;

    float rotation = 0.f;
    float angularVelocity = 0.f;
    float deceleration = 0.f;

	bool spinning = false;  // wheel initially not spinning

	int selected = 1;       // index of the currently selected segment
	int lastSelected = -1;  // index of the previously selected segment

	// variables for cursor blinking in editor
    float cursorTimer = 0.f;
    bool cursorVisible = true;

	// draw window with anti-aliasing
	sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;

    sf::RenderWindow window(
        sf::VideoMode({ 800, 600 }),
        "Wheel App",
        sf::Style::Default,
        sf::State::Windowed,
        settings
    );

    window.setFramerateLimit(60);

	sf::Clock clock;            // create a clock to track time between frames
	sf::Vector2f pointerTip;    // variable to store the position of the pointer tip for segment selection

	// create menu button
    std::vector<Button> menuOptions;

    Button menuButton(
        ButtonType::MenuIcon,
        font,
        "", // unused
        sf::Vector2f{ 40.f, 40.f },
        sf::Vector2f{ 10.f, 10.f }
    );

	// cached layouts to avoid recalculating every frame, updated on window resize
    TableLayout cachedLayout;
    ColourPickerLayout cachedPicker;

    sf::Vector2u lastWindowSize = window.getSize();
    size_t lastRowCount = editingSegments.size();

    bool layoutDirty = true;

	// menu option dimensions
    float buttonWidth = 200.f;
    float buttonHeight = 40.f;

	// position menu options below the menu button
    sf::Vector2f menuPos = menuButton.background.getPosition();

	// clear menu options vector in case of state changes
    menuOptions.clear();

    // editor state variable declarations
    int selectedRow = -1;
    bool editingLabel = false;
	bool textSelected = false;

	// variables for double-click detection on label editing
    sf::Clock clickClock;
    float lastClickTime = 0.f;
    const float doubleClickThreshold = 0.3f;

	// input buffer for weight editing
    std::string weightInput;

	// currently active field in the editor (label, weight, or colour)
    EditField activeField = EditField::None;

    // space to spin tooltip
    sf::Text tooltip(font);
    tooltip.setString("[Space] to spin");
    tooltip.setCharacterSize(18);
    tooltip.setFillColor(sf::Color(90, 90, 90));
    tooltip.setString("[Space] to spin");
    
	// decimal formatting for display
    std::ostringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);

	// colour picker layout declarations
    ColourPickerLayout picker;

    bool draggingHue = false;
    bool draggingSaturation = false;
    bool draggingValue = false;

	// create menu options
    for (int i = 0; i < 2; ++i)
    {
        menuOptions.emplace_back(
            ButtonType::Text,
            font,
            i == 0 ? "Load Wheel" :
            "Create / Edit Wheel",
            sf::Vector2f{ buttonWidth, buttonHeight },
            menuPos + sf::Vector2f{ 0.f, 50.f + i * (buttonHeight + 10.f) }
        );
    }

    while (window.isOpen())
    {

		float dt = clock.restart().asSeconds();     // calculate delta time

		// update cached layouts in case of window resizing
        sf::Vector2u currentSize = window.getSize();

        if (currentSize != lastWindowSize || editingSegments.size() != lastRowCount)
        {
            layoutDirty = true;
            lastWindowSize = currentSize;
            lastRowCount = editingSegments.size();
        }

        if (layoutDirty)
        {
            cachedLayout = getTableLayout(window);
            cachedPicker = getColourPickerLayout(window, static_cast<int>(editingSegments.size()), cachedLayout);
            layoutDirty = false;
        }

        if (state == AppState::CreateWheel)
        {
            // Update cursor blinking in editor
            cursorTimer += dt;

            if (cursorTimer >= 0.5f) // blink every 0.5s
            {
                cursorVisible = !cursorVisible;
                cursorTimer = 0.f;
            }
        }
        
		// update hover states for menu button and options
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        menuButton.updateHover(mousePos);

        for (auto& btn : menuOptions)
            btn.updateHover(mousePos);

		// event handling
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            // handle mouse clicks for menu interaction
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                auto* mouse = event->getIf<sf::Event::MouseButtonPressed>();

                if (mouse->button == sf::Mouse::Button::Left)
                {
                    if (state == AppState::MainView || state == AppState::MenuOpen)
                    {
                        if (menuButton.isHovered(mousePos))
                        {
                            if (state == AppState::MainView)
                                state = AppState::MenuOpen;
                            else
                                state = AppState::MainView;

                            continue;
                        }
                    }

                    if (state == AppState::MenuOpen)
                    {
                        bool optionClicked = false;

                        for (int i = 0; i < menuOptions.size(); ++i)
                        {
                            if (menuOptions[i].isHovered(mousePos))
                            {
                                if (i == 0)
                                {
                                    state = AppState::LoadWheel;
                                }
                                if (i == 1)
                                {
                                    editingSegments = wheel.segments;
                                    resetEditorState(selectedRow, activeField, weightInput, textSelected,
                                        showColourPicker, hue, saturation, value);

                                    state = AppState::CreateWheel;
                                }
                                optionClicked = true;
                                break;
                            }
                        }

                        // click outside menu to close
                        if (!optionClicked)
                        {
                            state = AppState::MainView;
                        }
                        continue;
                    }

                    // Screen buttons
                    if (state == AppState::CreateWheel ||
                        state == AppState::LoadWheel) {
                        if (menuButton.isHovered(mousePos))
                        {
                            state = AppState::MainView;

                            continue;
                        }
                    }
                    // handle creator table selection
                    if (state == AppState::CreateWheel)
                    {
                        const TableLayout& layout = cachedLayout;
                        const ColourPickerLayout& picker = cachedPicker;

                        bool clickedSomething = false;

						// check table row interactions
                        if (activeField == EditField::Weight &&
                            selectedRow >= 0 &&
                            selectedRow < editingSegments.size())
                        {
                            try
                            {
                                float v = std::stof(weightInput);
                                if (v > 0.f)
                                    editingSegments[selectedRow].weight = v;
                            }
                            catch (...) {
                                std::ostringstream ss;
                                ss.str("");
                                ss.clear();
                                ss << editingSegments[selectedRow].weight;
                                weightInput = ss.str();
                            }
                            // leave textSelected as false after commit
                            textSelected = false;
                        }

                        for (int i = 0; i < editingSegments.size(); ++i)
                        {
                            float y = layout.startY + (i + 1) * layout.rowHeight;

                            sf::FloatRect labelRect(
                                { layout.startX, y },
                                { layout.colLabel, layout.rowHeight }
                            );

                            sf::FloatRect colourRect(
                                { layout.startX + layout.colLabel, y },
                                { layout.colcolour, layout.rowHeight }
                            );

                            sf::FloatRect weightRect(
                                { layout.startX + layout.colLabel + layout.colcolour, y },
                                { layout.colWeight, layout.rowHeight }
                            );

                            float now = clickClock.getElapsedTime().asSeconds();
                            bool isDoubleClick = (now - lastClickTime) < doubleClickThreshold;

                            if (labelRect.contains(mousePos))
                            {
                                selectedRow = i;
                                activeField = EditField::Label;
                                showColourPicker = false;

                                textSelected = isDoubleClick;

                                lastClickTime = now;
                                clickedSomething = true;
                                break;
                            }

                            if (colourRect.contains(mousePos))
                            {
                                selectedRow = i;
                                activeField = EditField::Colour;
                                showColourPicker = true;
                                rgbToHsv(editingSegments[i].colour, hue, saturation, value);

                                textSelected = false;

                                lastClickTime = now;
                                clickedSomething = true;
                                break;
                            }

                            if (weightRect.contains(mousePos))
                            {
                                selectedRow = i;
                                activeField = EditField::Weight;

                                ss.str("");
                                ss.clear();
                                ss << editingSegments[i].weight;
                                weightInput = ss.str();

                                showColourPicker = false;
                                clickedSomething = true;

                                // select all text so next input replaces it
                                textSelected = true;

                                break;
                            }
                        }

                        // check picker interaction
                        if (showColourPicker)
                        {
                            float x = picker.x;
                            float y = picker.y;

                            float w = picker.width;
                            float h = picker.height;
                            float spacing = 10.f;

                            sf::FloatRect hueRect({ x + 10.f, y + 10.f }, { w, h });
                            sf::FloatRect satRect({ x + 10.f, y + 10.f + h + spacing }, { w, h });
                            sf::FloatRect valRect({ x + 10.f, y + 10.f + (h + spacing) * 2 }, { w, h });

                            if (hueRect.contains(mousePos))
                            {
                                draggingHue = true;
                                clickedSomething = true;
                            }
                            else if (satRect.contains(mousePos))
                            {
                                draggingSaturation = true;
                                clickedSomething = true;
                            }
                            else if (valRect.contains(mousePos))
                            {
                                draggingValue = true;
                                clickedSomething = true;
                            }
                        }

                        // click outside everything resets selection
                        if (!clickedSomething)
                        {
                            // focus loss: if we were editing a weight, commit it
                            if (activeField == EditField::Weight &&
                                selectedRow >= 0 &&
                                selectedRow < editingSegments.size())
                            {
                                try
                                {
                                    float v = std::stof(weightInput);
                                    if (v > 0.f)
                                        editingSegments[selectedRow].weight = v;
                                }
                                catch (...) {
                                    ss.str("");
                                    ss.clear();
                                    ss << editingSegments[selectedRow].weight;
                                    weightInput = ss.str();
                                }
                            }

                            selectedRow = -1;
                            activeField = EditField::None;
                            showColourPicker = false;
                        }
                    }
                }
            }

			// handle mouse release to stop dragging sliders
            if (event->is<sf::Event::MouseButtonReleased>())
            {
                draggingHue = false;
                draggingSaturation = false;
                draggingValue = false;
            }

            // handle text input for segment labels in createwheel state
            if (state == AppState::CreateWheel)
            {
                if (event->is<sf::Event::TextEntered>())
                {
                    auto* textEvent = event->getIf<sf::Event::TextEntered>();

                    if (activeField != EditField::None &&
                        selectedRow >= 0 &&
                        selectedRow < editingSegments.size())
                    {
                        // check for Enter (commit) or regular character input
                        if (textEvent->unicode == 13 || textEvent->unicode == 10)
                        {
                            // commit weight when Enter pressed while editing weight
                            if (activeField == EditField::Weight)
                            {
                                try
                                {
                                    float v = std::stof(weightInput);
                                    if (v > 0.f)
                                        editingSegments[selectedRow].weight = v;
                                }
                                catch (...) {
                                    ss.str("");
                                    ss.clear();
                                    ss << editingSegments[selectedRow].weight;
                                    weightInput = ss.str();
                                }

                                textSelected = false;
                            }
                        }
                        else
                        {
                            char entered = static_cast<char>(textEvent->unicode);

                            // label editing
                            if (activeField == EditField::Label)
                            {
                                if (entered == 8)
                                {
                                    if (textSelected)
                                    {
                                        editingSegments[selectedRow].label.clear();
                                        textSelected = false;
                                    }
                                    else if (!editingSegments[selectedRow].label.empty())
                                    {
                                        editingSegments[selectedRow].label.pop_back();
                                    }
                                }
                                else if (entered >= 32 && entered < 127)
                                {
                                    if (textSelected)
                                    {
                                        editingSegments[selectedRow].label.clear();
                                        textSelected = false;
                                    }

                                    editingSegments[selectedRow].label += entered;
                                }
                            }

                            // weight editing
                            else if (activeField == EditField::Weight)
                            {
                                if (entered == 8)
                                {
                                    if (textSelected)
                                    {
                                        weightInput.clear();
                                        textSelected = false;
                                    }
                                    else if (!weightInput.empty())
                                    {
                                        weightInput.pop_back();
                                    }
                                }
                                else if ((entered >= '0' && entered <= '9') || entered == '.')
                                {
                                    if (textSelected)
                                    {
                                        weightInput.clear();
                                        textSelected = false;
                                    }

                                    // allow only one decimal point
                                    if (entered != '.' || weightInput.find('.') == std::string::npos)
                                    {
                                        weightInput += entered;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // handle key presses
            if (event->is<sf::Event::KeyPressed>())
            {
                auto* key = event->getIf<sf::Event::KeyPressed>();
                if (!key) continue;

                if (activeField == EditField::Weight &&
                    selectedRow >= 0 &&
                    selectedRow < editingSegments.size())
                {
                    if (key->code == sf::Keyboard::Key::Escape)
                    {
                        // cancel edit and restore displayed value
                        ss.str("");
                        ss.clear();
                        ss << editingSegments[selectedRow].weight;
                        weightInput = ss.str();
                        textSelected = false;
                    }
                }

                // command mode
                else if (state == AppState::CreateWheel)
                {
                    // block shortcuts while editing text
                    if (activeField != EditField::None)
                        continue;

                    if (key->code == sf::Keyboard::Key::A)
                    {                        
                        const float HUE_STEP = 45.f; // degrees per new segment

                        float baseHue = hue;

                        if (!editingSegments.empty())
                        {
                            float lastHue, sat, val;
                            rgbToHsv(editingSegments.back().colour, lastHue, sat, val);
                            baseHue = lastHue;
                        }

                        float newHue = std::fmod(baseHue + HUE_STEP, 360.f);

                        hue = newHue;
                        saturation = 1.f;
                        value = 1.f;

                        sf::Color col = hsvToRgb(hue, saturation, value);

                        editingSegments.push_back({ "New", 1.f, col });
                    }
                    else if (key->code == sf::Keyboard::Key::R)
                    {
                        if (!editingSegments.empty())
                            editingSegments.pop_back();
                    }
                    else if (key->code == sf::Keyboard::Key::S)
                    {
                        normalizeWeights(editingSegments);
                        wheel.segments = editingSegments;
                        state = AppState::MainView;
                    }
                }
                // main view
                else if (state == AppState::MainView)
                {
                    if (key->code == sf::Keyboard::Key::Space && !spinning)
                    {
                        std::srand(static_cast<unsigned>(std::time(nullptr)));
                        rotation = static_cast<float>(std::rand() % 2160);
                        angularVelocity = 720.f + static_cast<float>(std::rand() % 180);
                        deceleration = 0.02f + static_cast<float>(std::rand()) / RAND_MAX * 0.02f;

                        spinning = true;
                    }
                }
            }
        
            if (event->is<sf::Event::Resized>())
            {
                auto* resize = event->getIf<sf::Event::Resized>();

                sf::FloatRect visibleArea(
                    { 0.f, 0.f },
                    { static_cast<float>(resize->size.x), static_cast<float>(resize->size.y) }
                );

                window.setView(sf::View(visibleArea));
            }

            // handle dragging of colour sliders in createwheel state
            if (state == AppState::CreateWheel && showColourPicker)
            {
                const ColourPickerLayout& picker = cachedPicker;

                float x = picker.x;
                float w = picker.width;

                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                auto clamp01 = [](float v)
                    {
                        return std::max(0.f, std::min(1.f, v));
                    };

                float t = clamp01((mousePos.x - (x + 10.f)) / w);

                if (draggingHue)
                    hue = t * 360.f;

                if (draggingSaturation)
                    saturation = t;

                if (draggingValue)
                    value = t;

                if (selectedRow >= 0 && selectedRow < editingSegments.size())
                {
                    editingSegments[selectedRow].colour = hsvToRgb(hue, saturation, value);
                }
            }
        }

		// clear the window with a white-ish background
        window.clear(sf::Color(228,228,228,255));

    	// get latest window size and calculate wheel centre and pointer centre for drawing
        sf::Vector2u windowSize = window.getSize();

        sf::Vector2f center = {
            windowSize.x * 0.5f,
            windowSize.y * 0.5f
        };
        
        sf::Vector2f pointerPos = {
            windowSize.x * 0.5f,
            windowSize.y * 0.05f
        };

		if (spinning)
        {
            // Apply rotation
            rotation += angularVelocity * dt;

            // Linear deceleration
            angularVelocity -= deceleration * dt;

			float effectiveDeceleration = deceleration * 3600.f; // tuned factor to make deceleration consistent across different frame rates

            angularVelocity -= effectiveDeceleration * dt;

            // Stop condition
            if (angularVelocity <= 0.99f)
            {
                    spinning = false;
                    angularVelocity = 0.f;

                int selected = getSelectedSegment(
                    wheel.segments,
                    center,
                    pointerTip,
                    rotation
                );

                if (selected >= 0 && selected < wheel.segments.size())
                {
					continue; // placeholder for any future action on selected segment
                }

            }
        }

        float radius = std::min(windowSize.x, windowSize.y) * 0.3f; // radius declaration moved here to be accessible in all states

        switch (state)
        {
            case AppState::MainView:
            {
                drawWheel(window, wheel.segments, radius, center, rotation, font);
                pointerTip = drawPointer(window, radius, center);
                drawMenuButton(window, menuButton);
                if (!spinning)
                {                    
                    tooltip.setPosition({
                        center.x - tooltip.getGlobalBounds().size.x * 0.5f,
                        center.y + radius + 30.f
                        });

                    window.draw(tooltip);
                }
                break;
            }

            case AppState::MenuOpen:
            {
                drawWheel(window, wheel.segments, radius, center, rotation, font);
                pointerTip = drawPointer(window, radius, center);
                drawMenuButton(window, menuButton);
                drawMenu(window, menuOptions, font);
                if (!spinning)
                {
                    tooltip.setPosition({
                        center.x - tooltip.getGlobalBounds().size.x * 0.5f,
                        center.y + radius + 30.f
                    });
                    window.draw(tooltip);
                }
                break;
            }

            case AppState::CreateWheel:
            {
                drawCreateWheelUI(
                    window,
                    editingSegments,
                    editingWheelName,
                    font,
                    selectedRow,
                    activeField,
                    weightInput,
                    cursorVisible,
                    textSelected,
                    cachedLayout,
                    cachedPicker
                );
                drawMenuButton(window, menuButton);
                break;
            }

            case AppState::LoadWheel:
            {
                drawLoadWheelUI(window, font);
                drawMenuButton(window, menuButton);
                break;
            }
        }

        window.display();
    }

    return 0;
}