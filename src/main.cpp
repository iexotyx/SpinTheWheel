#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <windows.h>

#include "AppStates.h"
#include "EditorState.h"
#include "Renderer.h"
#include "Input.h"
#include "Wheel.h"
#include "Shortcuts.h"

int main()
{
    // load font
    sf::Font font;

    // debug font loading
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        return 1;
    }

    // seed random (repeats every time the wheel is spun)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

	AppState state = AppState::MainView;    // start in main view

    Wheel wheel;

    EditorState editor;

    WheelSpinState spin;

    // initial wheel
    wheel.name = "Basic Wheel";

    wheel.segments = {
        {"Red", 1, sf::Color::Red},
        {"Green", 1, sf::Color::Green},
        {"Blue", 1, sf::Color::Blue}
    };

    int selected = 1;       // index of the currently selected segment
	int lastSelected = -1;  // index of the previously selected segment

	// draw window with anti-aliasing
	sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;

    sf::RenderWindow window(
        sf::VideoMode({ 1000, 800 }),
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
    size_t lastRowCount = editor.segments.size();

    bool layoutDirty = true;

	// menu option dimensions
    float buttonWidth = 200.f;
    float buttonHeight = 40.f;

	// position menu options below the menu button
    sf::Vector2f menuPos = menuButton.background.getPosition();

	// clear menu options vector in case of state changes
    menuOptions.clear();

	// variables for double-click detection on label editing
    sf::Clock clickClock;
    const float doubleClickThreshold = 0.3f;

    // space to spin tooltip
    sf::Text tooltip(font);
    tooltip.setString("[Space] to spin");
    tooltip.setCharacterSize(18);
    tooltip.setFillColor(sf::Color(90, 90, 90));
    
	// decimal formatting for display
    std::ostringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);

	// colour picker layout declarations
    ColourPickerLayout picker;

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

        if (currentSize != lastWindowSize || editor.segments.size() != lastRowCount)
        {
            layoutDirty = true;
            lastWindowSize = currentSize;
            lastRowCount = editor.segments.size();
        }

        if (layoutDirty)
        {
            cachedLayout = getTableLayout(window);
            cachedPicker = getColourPickerLayout(static_cast<int>(editor.segments.size()), cachedLayout);
            layoutDirty = false;
            cachedPicker.gradientsDirty = true;
        }

        if (state == AppState::CreateWheel)
        {
            // Update cursor blinking in editor
            editor.cursorTimer += dt;

            if (editor.cursorTimer >= 0.5f) // blink every 0.5s
            {
                editor.cursorVisible = !editor.cursorVisible;
                editor.cursorTimer = 0.f;
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
                                    editor.segments = wheel.segments;
                                    editor.wheelName = wheel.name;
                                    editor.resetEditorState();

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

                        handleEditorMouseClick(
                            mousePos,
                            window,
                            cachedLayout,
                            cachedPicker,
                            editor,
                            clickClock,
                            doubleClickThreshold
                        );
                    }
                }
            }

			// handle mouse scroll for scrolling through segments in createwheel state
            if (event->is<sf::Event::MouseWheelScrolled>())
            {
                auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>();

                if (state == AppState::CreateWheel)
                {
                    handleEditorScroll(
                        wheel->delta,
                        window,
                        cachedLayout,
                        editor
                    );
                }
            }

			// handle mouse release to stop dragging sliders
            if (event->is<sf::Event::MouseButtonReleased>())
            {
                editor.draggingHue = false;
                editor.draggingSaturation = false;
                editor.draggingValue = false;
            }

            // handle text input for segment labels in createwheel state
            if (state == AppState::CreateWheel)
            {
                if (event->is<sf::Event::TextEntered>())
                {
                    auto* textEvent =
                        event->getIf<sf::Event::TextEntered>();

                    handleEditorTextInput(
                        *textEvent,
                        editor
                    );
                }
            }

            // handle key presses for specific events
            if (event->is<sf::Event::KeyPressed>())
            {
                auto* key =
                    event->getIf<sf::Event::KeyPressed>();

                if (!key)
                    continue;

                if (handleShortcut(
                    *key,
                    state,
                    editor,
                    wheel,
                    spin,
                    ss))
                {
                    continue;
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
            if (state == AppState::CreateWheel && editor.showColourPicker)
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

                if (editor.draggingHue)
                    editor.hue = t * 360.f;

                if (editor.draggingSaturation)
                    editor.saturation = t;

                if (editor.draggingValue)
                    editor.value = t;

                if (editor.hasSelectedRow())
                {
                    editor.segments[editor.selectedRow].colour = hsvToRgb(editor.hue, editor.saturation, editor.value);
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

		if (spin.spinning)
        {
            // Apply rotation
            spin.rotation += spin.angularVelocity * dt;

			float effectiveDeceleration = spin.deceleration * 3600.f; // tuned factor to make deceleration consistent across different frame rates

            spin.angularVelocity -= effectiveDeceleration * dt;

            // Stop condition
            if (spin.angularVelocity <= 0.99f)
            {
                    spin.spinning = false;
                    spin.angularVelocity = 0.f;

                int selected = getSelectedSegment(
                    wheel.segments,
                    center,
                    pointerTip,
                    spin.rotation
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
                drawWheel(window, wheel.segments, radius, center, spin.rotation, font);
                pointerTip = drawPointer(window, radius, center);
                drawWheelName(font, wheel, window, center);
                drawMenuButton(window, menuButton);
                if (!spin.spinning)
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
                drawWheel(window, wheel.segments, radius, center, spin.rotation, font);
                pointerTip = drawPointer(window, radius, center);
                drawMenuButton(window, menuButton);
                drawMenu(window, menuOptions, font);
                if (!spin.spinning)
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
                    font,
                    editor,
                    cachedLayout,
                    cachedPicker);

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