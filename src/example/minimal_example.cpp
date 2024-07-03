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

    // The config. is optional if your assets are packaged according to the defaults:
    // sfw::GUI gui(window, { .basePath = "asset/",
    //                        .textureFile = "texture/default.png",
    //                        .fontFile = "font/default.ttf",
    //                        /* ... */ });

    // Add a button
    gui.add(sfw::Button("Close!", [&] { gui.close(); }));

    // Event loop (blocking variant)
    while (gui) // Or while(gui.active()), but we're in a minimal example. ;)
    {
        // Render the GUI
        gui.render();

        // Actually show the (updated) window content
        window.display();

        // Process events, one by one, waiting until the next one comes
        if (const std::optional<sf::Event> event = window.waitEvent(); event)
	{
            gui.process(*event); // Pass it to the GUI!

            // Your own custom event processing here...
        }
    }

    return 0;
}
