#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "Segment.h"

// Segment ID based on position of the pointer
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

    float pointerAngle = std::atan2(dy, dx) * 180.f / 3.14159265f;

    if (pointerAngle < 0)
        pointerAngle += 360.f;

    // Total weight
    float totalWeight = 0.f;
    for (const auto& s : segments)
        totalWeight += s.weight;

    // Walk segments
    float currentAngle = rotation; // match draw start

    for (int i = 0; i < segments.size(); ++i)
    {
        float segmentAngle = (segments[i].weight / totalWeight) * 360.f;

        float start = std::fmod(currentAngle, 360.f);
        if (start < 0) start += 360.f;

        float end = std::fmod(currentAngle + segmentAngle, 360.f);
        if (end < 0) end += 360.f;

        // Handle wrap-around
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

void drawWheel(sf::RenderWindow& window,
    const std::vector<Segment>& segments,
    float radius,
    sf::Vector2f center,
    float rotation)
{

    float totalWeight = 0.f;
    for (const auto& s : segments)
        totalWeight += s.weight;

    float startAngle = rotation;

    for (const auto& s : segments)
    {
        float angle = (s.weight / totalWeight) * 360.f;

        sf::VertexArray slice(sf::PrimitiveType::TriangleFan);

        // Center vertex
        sf::Vertex centerVertex;
        centerVertex.position = center;
        centerVertex.color = s.color;
        slice.append(centerVertex);

        int points = 30;

        for (int i = 0; i <= points; ++i)
        {
            float currentAngle = startAngle + angle * (float(i) / points);
            float rad = currentAngle * 3.14159265f / 180.f;

            sf::Vector2f point = {
                center.x + std::cos(rad) * radius,
                center.y + std::sin(rad) * radius
            };

            sf::Vertex v;
            v.position = point;
            v.color = s.color;
            slice.append(v);
        }

        window.draw(slice);

        startAngle += angle;
    }
}

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

int main()
{
    // load font
    sf::Font font;

    // debug font loading
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cout << "Failed to load font\n";
    }

    enum class AppState
    {
        MainView,      // wheel
        MenuOpen,      // dropdown visible
        CreateWheel,   // editor screen
        EditWheel,     // editor with existing data
        LoadWheel      // file selection screen
    };

	AppState state = AppState::MainView;    // start in main view

	// menu button types
    enum class ButtonType
    {
        Text,
        MenuIcon
    };

	// simple button struct for menu options
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

				// create 3 horizontal bars for menu icon
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

	// wheel data structure
    struct Wheel
    {
        std::string name;
        std::vector<Segment> segments;
    };

    Wheel wheel;

	// example segments these will eventually be loaded from a file or created by the user
    wheel.name = "Test Wheel";

    wheel.segments = {
        {"R", 1, sf::Color::Red},
        {"G", 1, sf::Color::Green},
        {"B", 1, sf::Color::Blue}
    };

    float rotation = 0.f;
    float angularVelocity = 0.f;
    float deceleration = 0.f;
	bool spinning = false;                                                              // wheel initially not spinning

	int selected = 1;       // index of the currently selected segment
	int lastSelected = -1;  // index of the previously selected segment

	// draw window with anti-aliasing
	sf::ContextSettings settings;
    settings.antiAliasingLevel = 16;

    sf::RenderWindow window(
        sf::VideoMode({ 800, 600 }),
        "Wheel App",
        sf::Style::Default,
        sf::State::Windowed,
        settings
    );

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

    sf::Vector2f menuPos = menuButton.background.getPosition();

    float buttonWidth = 200.f;
    float buttonHeight = 40.f;

    menuOptions.clear();

    for (int i = 0; i < 3; ++i)
    {
        menuOptions.emplace_back(
            ButtonType::Text,
            font,
            i == 0 ? "Load Wheel" :
            i == 1 ? "Create New Wheel" :
            "Edit Wheel",
            sf::Vector2f{ buttonWidth, buttonHeight },
            menuPos + sf::Vector2f{ 0.f, 50.f + i * (buttonHeight + 10.f) }
        );
    }

    while (window.isOpen())
    {

		float dt = clock.restart().asSeconds();                                             // calculate delta time
        
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
                    if (menuButton.isHovered(mousePos))
                    {
                        if (state == AppState::MainView)
                            state = AppState::MenuOpen;
                        else if (state == AppState::MenuOpen)
                            state = AppState::MainView;
                    }

                    if (state == AppState::MenuOpen)
                    {
                        for (int i = 0; i < menuOptions.size(); ++i)
                        {
                            if (menuOptions[i].isHovered(mousePos))
                            {
                                if (i == 0) state = AppState::LoadWheel;
                                if (i == 1) state = AppState::CreateWheel;
                                if (i == 2) state = AppState::EditWheel;
                            }
                        }
                    }
                }
            }

			// handle keyboard input for spinning the wheel
            if (event->is<sf::Event::KeyPressed>())
            {
                const auto* key = event->getIf<sf::Event::KeyPressed>();

				if (key && key->code == sf::Keyboard::Key::Space && !spinning)      // if space is pressed and wheel is not already spinning, start spinning
                {
                    // seed random number generator
                    std::srand(static_cast<unsigned>(std::time(nullptr)));
                    rotation = static_cast<float>(std::rand() % 2160);                           // random initial Position (0-360 degrees)
					angularVelocity = 720.f + static_cast<float>(std::rand() % 180);            // random initial Speed (720-900 degrees per second)
                    deceleration = 0.02f + static_cast<float>(std::rand()) / RAND_MAX * 0.02f;  // random deceleration (-0.02-0.04 degrees per frame)

                    spinning = true;
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

        }

        window.clear(sf::Color::White);

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

		// draw menu button
        menuButton.draw(window);

        if (state == AppState::MenuOpen)
        {
            for (const auto& btn : menuOptions)
                btn.draw(window);
        }

		// draw the wheel
        float radius = std::min(windowSize.x, windowSize.y) * 0.3f;

		drawWheel(window, wheel.segments, radius, center, rotation);

        // draw the pointer
        pointerTip = drawPointer(window, radius, center);

        if (spinning)
        {
            // Apply rotation
            rotation += angularVelocity * dt;

            // Linear deceleration
            angularVelocity -= deceleration * dt;

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
                    std::cout << "Selected: "
                        << wheel.segments[selected].label
                        << std::endl;
                }

            }
            else
            {
                angularVelocity -= deceleration;
            }
        }

        window.display();

    }

    
}