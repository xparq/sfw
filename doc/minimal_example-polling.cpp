#include "sfw/GUI.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    //
    // Normal SFML app init...
    //
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFW Minimal (polling loop)", sf::Style::Titlebar);
    window.setFramerateLimit(30);

    //
    // GUI Setup...
    //
    sfw::GUI gui(window);

    // The config. is optional if assets are packaged according to the defaults:
    // sfw::GUI gui(window, { .basePath = "asset/",
    //                        .textureFile = "texture/default.png",
    //                        .fontFile = "font/default.ttf",
    //                        /* ... */ };

    // Add a button
    gui.add(sfw::Button("Close!", [&] { window.close(); }));

    // Main event loop (polling variant; note: the GUI will be redrawn in each frame, regardless of changes)
    while (window.isOpen())
    {
        // Render the GUI
        gui.render();

        // Actually show the (updated) window content
        window.display();

        // Process events in the queue (each one, to avoid possible congestion)
        for (sf::Event event; window.pollEvent(event) && event.type != sf::Event::Closed; )
        {
            // Pass events to the GUI
            gui.process(event);
        }
    }

    return 0;
}
