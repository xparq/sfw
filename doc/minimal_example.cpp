#include "sfw/Gui.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFW Minimal", sf::Style::Close);

    // The main GUI manager object
    gui::Main gui(window);

    // Load the visual styling template (sprite-sheet) image, and some font
    gui::Theme::loadTexture("demo/texture-default.png");
    gui::Theme::loadFont("demo/tahoma.ttf");

    // Create a button
    gui.add((new gui::Button("Close!"))->setCallback([&] { window.close(); }));
    // Or, verbosely:
    // auto button = new gui::Button("Close!");
    // button->setCallback([&] { window.close(); });
    // gui.add(button);

    // Event loop (blocking variant, with waitEvent)
    while (window.isOpen())
    {
        // Show content anew, or as updated in the previous cycle
        window.display();

        if (sf::Event event; window.waitEvent(event))
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
    }

    return 0;
}
