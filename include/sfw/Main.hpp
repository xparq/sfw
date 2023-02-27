#ifndef GUI_MAIN_HPP
#define GUI_MAIN_HPP

#include "sfw/Layouts/VBoxLayout.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Cursor.hpp>

namespace gui
{

/**
 * Entry point for the GUI.
 * The Main object is actually a VBoxLayout, with a general onEvent handler.
 */
class Main: public gui::VBoxLayout
{
public:
    Main(sf::RenderWindow& window);

    /**
     * Handle an SFML event and send it to widgets
     */
    void onEvent(const sf::Event& event);

private:
    /**
     * Get mouse cursor relative position
     * @param x: absolute x position from the event
     * @param y: absolute y position from the event
     * @return relative mouse position
     */
    sf::Vector2f convertMousePosition(int x, int y) const;

    /**
     * Update the cursor type on the RenderWindow
     */
    void setMouseCursor(sf::Cursor::Type cursorType) override;

    sf::RenderWindow& m_window;
    sf::Cursor::Type m_cursorType;
};

}

#endif // GUI_MAIN_HPP
