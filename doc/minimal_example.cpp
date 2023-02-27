#include "sfw/Gui.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Widgets", sf::Style::Close);

    // The main GUI manager object
    gui::Main gui(window);

    // Load the visual styling template (sprite-sheet) image
    gui::Theme::loadTexture("demo/texture-default.png");

    gui::Theme::loadFont("demo/tahoma.ttf");

    // Create a button
    auto button = new gui::Button("My button");
    button->setCallback([] { std::cout << "click!" << std::endl; });
    gui.add(button);

    // The main event loop (Note: the entire GUI will be redrawn in every frame.)
    while (window.isOpen())
    {
        // Process each event currently in the queue (to avoid possible congestion)
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
