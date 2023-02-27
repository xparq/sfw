#include "sfw/Gui.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFW Minimal (polling loop)", sf::Style::Close);
    window.setFramerateLimit(30);

    // The main GUI manager object
    gui::Main gui(window);

    // Load the visual styling template (sprite-sheet) image
    gui::Theme::loadTexture("demo/texture-default.png");

    gui::Theme::loadFont("demo/tahoma.ttf");

    // Create a button
    auto button = new gui::Button("Close!");
    button->setCallback([&] { window.close(); });
    gui.add(button);

    // Main event loop (polling variant; note: the entire GUI will be redrawn in every frame)
    while (window.isOpen())
    {
        // Process events in the queue (each one, to avoid possible congestion)
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Send events to the GUI!
            gui.onEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window with the theme's background color
        window.clear(gui::Theme::windowBgColor);

        // Render the GUI
        window.draw(gui);

        // Show the refreshed window
        window.display();
    }

    return 0;
}
