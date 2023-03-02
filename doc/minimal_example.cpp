#include "sfw/GUI.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFW Minimal", sf::Style::Close);

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

    // Event loop (blocking variant, with waitEvent)
    while (window.isOpen())
    {
        // Show content anew, or as updated in the previous cycle
        window.display();

        if (sf::Event event; window.waitEvent(event))
        {
            // Send events to the GUI!
            gui.process(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Render the GUI (will clear the window first, by default)
        gui.render();
    }

    return 0;
}
