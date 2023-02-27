#include "sfw/Gui.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Widgets", sf::Style::Close);

    // Main (root) gui manager object
    gui::Main gui(window);

    gui::Theme::loadFont("demo/tahoma.ttf");
    gui::Theme::loadTexture("demo/texture-default.png");

    // Create a button
    gui::Button* button = new gui::Button("My button");
    button->setCallback([] {
        std::cout << "click!" << std::endl;
    });
    gui.add(button);

    // The main event loop
    // (Note: this simple polling loop redraws the entire GUI in every frame, even if nothing has changed.)
    while (window.isOpen())
    {
        // Process events currently in the queue
        // (This inner loop allows eating multiple events in one frame, to avoid possible congestion!)
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Send events to the gui
            gui.onEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Optional: clear window with theme background color
        window.clear(gui::Theme::windowBgColor);

        // Render the gui
        window.draw(gui);

        // Update the window
        window.display();
    }

    return 0;
}
