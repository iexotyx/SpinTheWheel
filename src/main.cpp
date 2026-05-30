#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <filesystem>

#include "AppStates.h"
#include "AppContext.h"
#include "Layouts.h"
#include "Shortcuts.h"
#include "logic/Spin.h"
#include "rendering/FileBrowserRenderer.h"
#include "rendering/WheelRenderer.h"
#include "rendering/CreateWheelRenderer.h"
#include "rendering/MenuRenderer.h"
#include "inputs/EventRouter.h"
#include "logic/EditorUpdate.h"

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

    AppContext app;

    sf::ContextSettings windowSettings;

    sf::Clock clock;            // create a clock to track time between frames

    sf::Vector2f pointerTip;    // variable to store the position of the pointer tip for segment selection

    std::vector<Button> menuOptions;    // create menu button

    windowSettings.antiAliasingLevel = 4;

    sf::RenderWindow window(
        sf::VideoMode({ 1000, 800 }),
        "Wheel App",
        sf::Style::Default,
        sf::State::Windowed,
        windowSettings
    );

    window.setFramerateLimit(60);

	// get window size for layout calculations
    sf::Vector2u windowSize = window.getSize();

	// calculate center of the window for wheel drawing
    sf::Vector2f center = {
        windowSize.x * 0.5f,
        windowSize.y * 0.5f
    };

    float radius = std::min(windowSize.x, windowSize.y) * 0.3f; // radius declaration

    // pointer tip is a fixed distance from the center
    sf::Vector2f pointerPos = {
        windowSize.x * 0.5f,
        windowSize.y * 0.05f
    };

    std::filesystem::create_directories(
        app.appSettings.wheelDirectory);

    std::filesystem::create_directories(
        app.appSettings.treeDirectory);

	// create menu button
    Button menuButton(
        ButtonType::MenuIcon,
        font,
        "", // unused
        sf::Vector2f{ 40.f, 40.f },
        sf::Vector2f{ 10.f, 10.f }
    );

    // initial wheel
    app.wheel.name = "Basic Wheel";

    app.wheel.segments = {
        {"Red", 1, sf::Color::Red},
        {"Green", 1, sf::Color::Green},
        {"Blue", 1, sf::Color::Blue}
    };

	// menu option dimensions
    float buttonWidth = 200.f;
    float buttonHeight = 40.f;

	// position menu options below the menu button
    sf::Vector2f menuPos = menuButton.background.getPosition();

	// variables for double-click detection on label editing
    sf::Clock clickClock;
    const float doubleClickThreshold = 0.3f;

    // space to spin tooltip
    sf::Text tooltip(font);
    tooltip.setString("[Space] to spin");
    tooltip.setCharacterSize(18);
    tooltip.setFillColor(sf::Color(90, 90, 90));
    tooltip.setPosition({
            center.x - tooltip.getGlobalBounds().size.x * 0.5f,
            center.y + radius + 30.f
        });

	// decimal formatting for display
    std::ostringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);

	// create menu options
    const std::vector<std::string> menuLabels =
    {
        "Main View",
        "Load Wheel",
        "Create / Edit Wheel"
    };

    for (int i = 0; i < menuLabels.size(); ++i)
    {
        menuOptions.emplace_back(
            ButtonType::Text,
            font,
            menuLabels[i],
            sf::Vector2f{ buttonWidth, buttonHeight },
            menuPos + sf::Vector2f{
                0.f,
                50.f + i * (buttonHeight + 10.f)
            }
        );
    }

    while (window.isOpen())
    {

		float dt = clock.restart().asSeconds();     // calculate delta time

        TableLayout layout =
            getTableLayout(window);

        ColourPickerLayout picker =
            getColourPickerLayout(
                static_cast<int>(app.editor.segments.size()),
                layout
            );

		// update hover states for menu button and options
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        menuButton.updateHover(mousePos);

        for (auto& btn : menuOptions)
            btn.updateHover(mousePos);

        radius = std::min(windowSize.x, windowSize.y) * 0.3f;

		// event handling
        while (auto event = window.pollEvent())
        {
            handleEvent(
                *event,
                window,
                app,
				menuButton,
				menuOptions,
                layout,
                picker,
                clickClock,
                doubleClickThreshold,
				tooltip,
                radius,
                ss
            );
        }

		if (app.spin.spinning)
        {
            SpinResult spinResult =
                updateSpin(
                    app,
                    center,
                    pointerTip,
                    dt
                );

            if (spinResult.finished)
            {
                //spinResult.selectedIndex;

                // wheel outcome logic for decision trees
            }
        }

        // clear the window with a white-ish background
        window.clear(sf::Color(228, 228, 228, 255));

        switch (app.state)
        {

            case AppState::MainView:
            {
                drawWheel(window, app.wheel.segments, radius, center, app.spin.rotation, font);
                pointerTip = drawPointer(window, radius, center);
                drawWheelName(font, app.wheel, window, center);
                
				if (!app.spin.spinning)
				{
					window.draw(tooltip);
				}

                break;
            }

            case AppState::EditWheel:
            {
                drawEditWheelUI(
                    window,
                    font,
                    app.editor,
                    layout,
                    picker);

                updateEditWheel(
                    window,
                    app,
                    picker,
                    dt);

                break;
            }

            case AppState::FileBrowser:
            {
                drawFileBrowserUI(window, font, app.fileBrowser);
                break;
            }

        }

        drawMenuButton(window, menuButton);

        if (app.menuOpen)
        {
            drawMenu(window, menuOptions, font);
        }

        window.display();
    }

    return 0;
}