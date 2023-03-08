#include "sfw/GUI.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    //
    // Normal SFML app init...
    //
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFW Minimal", sf::Style::Titlebar);

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

    // Event loop (blocking variant)
    while (window.isOpen())
    {
        // Render the GUI
        gui.render();

        // Actually show the (updated) window content
        window.display();

        // Process events, one by one, waiting until the next one comes
        if (sf::Event event; window.waitEvent(event))
        {
            // Pass events to the GUI
            gui.process(event);
        }
    }

    return 0;
}
