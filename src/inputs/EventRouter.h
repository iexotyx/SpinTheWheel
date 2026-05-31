#pragma once

#include <filesystem>

#include "persistence/FileBrowser.h"
#include "persistence/AppSettings.h"
#include "inputs/EditorInput.h"
#include "inputs/FileBrowserInput.h"

inline bool handleShortcut(
    const sf::Event::KeyPressed& key,
	AppContext& app,
    std::ostringstream& ss)
{
    if (key.code == sf::Keyboard::Key::Escape)
    {
        shortcutCancelWeightEdit(
            app.editor,
            ss
        );

        return true;
    }

    if (app.state == AppState::EditWheel &&
        app.editor.activeField == EditField::None)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))
        {
            if (key.code == sf::Keyboard::Key::S)
            {
                if (app.editor.activeField ==
                    EditField::Weight)
                {
                    app.editor.commitWeightEdit();
                }

                normalizeWeights(
                    app.editor.segments);

                saveEditorWheel(
                    app.editor,
                    app.wheel,
                    app.appSettings.wheelDirectory);

                return true;
            }
        }

        if (key.code ==
            sf::Keyboard::Key::A)
        {
            shortcutAddSegment(app.editor);
            return true;
        }

        if (key.code ==
            sf::Keyboard::Key::R)
        {
            shortcutRemoveSegment(app.editor);
            return true;
        }

        if (key.code ==
            sf::Keyboard::Key::S)
        {
            shortcutSaveWheel(
                app.editor,
                app.wheel,
                app.state
            );

            return true;
        }
    }

    if (app.state == AppState::MainView)
    {
        if (key.code ==
            sf::Keyboard::Key::Space)
        {
            shortcutSpinWheel(app.spin);
            return true;
        }
    }

    if (app.state == AppState::FileBrowser)
    {
        if (key.code ==
            sf::Keyboard::Key::Enter)
        {
            if (loadSelectedWheel(
                app.fileBrowser,
                app.wheel))
            {
                app.state =
                    AppState::MainView;
            }

            return true;
        }
    }

    return false;
}

// main event handling function, handles all events and routes them to specific handlers based on app state and event type
void handleEvent(
    const sf::Event& event,
    sf::RenderWindow& window,
    AppContext& app,
    Button& menuButton,
    std::vector<Button>& menuOptions,
    Button& loadWheelButton,
    Button& deleteWheelButton,
    const TableLayout& layout,
    const ColourPickerLayout& picker,
    sf::Clock& clickClock,
    float doubleClickThreshold,
    sf::Text& tooltip,
    std::ostringstream& ss)
{
    if (event.is<sf::Event::Closed>())
        window.close();

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (event.is<sf::Event::MouseButtonPressed>())
    {
        auto* mouse = event.getIf<sf::Event::MouseButtonPressed>();

        if (mouse->button == sf::Mouse::Button::Left)
        {
            if (menuButton.isHovered(mousePos))
            {
                app.menuOpen = !app.menuOpen;
                return;
            }

            if (app.menuOpen)
            {
                for (int i = 0; i < menuOptions.size(); ++i)
                {
                    if (!menuOptions[i].isHovered(mousePos))
                        continue;

                    switch (i)
                    {
                    case 0: // Main View
                        app.state = AppState::MainView;
                        break;

                    case 1: // Load Wheel
                        app.fileBrowser.mode =
                            FileDialogMode::LoadWheel;

                        app.fileBrowser.currentDirectory =
                            app.appSettings.wheelDirectory;

                        refreshFileBrowser(
                            app.fileBrowser);

                        app.state =
                            AppState::FileBrowser;
                        break;

                    case 2: // Create / Edit Wheel

                        app.editor.segments = app.wheel.segments;
                        app.editor.wheelName = app.wheel.name;
                        app.editor.resetEditorState();

                        app.state = AppState::EditWheel;
                        break;
                    }

                    app.menuOpen = false;
                    return;
                }

                // clicked outside menu
                app.menuOpen = false;
                return;
            }

            // handle creator table selection
            if (app.state == AppState::EditWheel)
            {
                handleEditorMouseClick(
                    mousePos,
                    window,
                    layout,
                    picker,
                    app.editor,
                    clickClock,
                    doubleClickThreshold
                );
            }
            if (app.state == AppState::FileBrowser)
            {
                if (app.fileBrowser.selectedIndex >= 0)
                {
                    if (loadWheelButton.isHovered(mousePos))
                    {
                        if (loadSelectedWheel(
                            app.fileBrowser,
                            app.wheel))
                        {
                            app.state =
                                AppState::MainView;
                        }

                        return;
                    }

                    if (deleteWheelButton.isHovered(mousePos))
                    {
                        deleteSelectedWheel(
                            app.fileBrowser);

                        return;
                    }
                }

                handleFileBrowserClick(
                    mousePos,
                    window,
                    app.fileBrowser,
                    layout);
            }
        }
    }

    // handle mouse scroll for scrolling through segments in createwheel state
    if (event.is<sf::Event::MouseWheelScrolled>())
    {
        auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>();

        if (app.state == AppState::EditWheel)
        {
            handleEditorScroll(
                wheel->delta,
                window,
                layout,
                app.editor
            );
        }
        if (app.state == AppState::FileBrowser)
        {
            handleFileBrowserScroll(
                wheel->delta,
                window,
                layout,
                app.fileBrowser);
        }
    }

    // handle mouse release to stop dragging sliders
    if (event.is<sf::Event::MouseButtonReleased>())
    {
        app.editor.draggingHue = false;
        app.editor.draggingSaturation = false;
        app.editor.draggingValue = false;
    }

    // handle text input for segment labels in createwheel state
    if (app.state == AppState::EditWheel)
    {
        if (event.is<sf::Event::TextEntered>())
        {
            auto* textEvent =
                event.getIf<sf::Event::TextEntered>();

            handleEditorTextInput(
                *textEvent,
                app.editor
            );
        }
    }

    // handle key presses for specific events
    if (event.is<sf::Event::KeyPressed>())
    {
        auto* key =
            event.getIf<sf::Event::KeyPressed>();

        handleShortcut(
            *key,
            app,
            ss);
    }

    if (event.is<sf::Event::Resized>())
    {
        auto* resize = event.getIf<sf::Event::Resized>();

        sf::FloatRect visibleArea(
            { 0.f, 0.f },
            { static_cast<float>(resize->size.x), static_cast<float>(resize->size.y) }
        );

        window.setView(sf::View(visibleArea));

        // get latest window size and calculate wheel centre and pointer centre
        sf::Vector2u windowSize = window.getSize();

        updateWheelPosition(
			windowSize,
            app.wheel,
			tooltip);
    }
}