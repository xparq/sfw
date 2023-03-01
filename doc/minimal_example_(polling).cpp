#include "sfw/GUI.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFW Minimal (polling loop)", sf::Style::Close);
    window.setFramerateLimit(30);

    // The main GUI manager object
    sfw::GUI gui(window);

    // Load the visual styling template (sprite-sheet) image, and some font
    sfw::Theme::loadTexture("demo/texture-default.png");
    sfw::Theme::loadFont("demo/tahoma.ttf");

    // Create a button
    gui.add(new sfw::Button("Close!"))->setCallback([&] { window.close(); });
    // Or, verbosely:
    // auto button = new sfw::Button("Close!");
    // button->setCallback([&] { window.close(); });
    // gui.add(button);

    // Main event loop (polling variant; note: the GUI will be redrawn in each frame, regardless of changes)
    while (window.isOpen())
    {
        // Process events in the queue (each one, to avoid possible congestion)
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Send events to the GUI!
            gui.process(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window with the theme's background color
        window.clear(sfw::Theme::windowBgColor);

        // Render the GUI
        gui.render();

        // Show the refreshed window
        window.display();
    }

    return 0;
}
