#ifndef GUI_MAIN_HPP
#define GUI_MAIN_HPP

#include "sfw/Theme.hpp"
#include "sfw/Gfx/Render.hpp"
#include "sfw/Layouts/VBoxLayout.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Cursor.hpp>

#include <string>
#include <map>

namespace sfw
{

/**
 * Entry point for the GUI.
 * The Main object is actually a VBoxLayout, with a catch-all event handler.
 */
class GUI: public VBoxLayout
{
public:
    GUI(sf::RenderWindow& window, const sfw::Theme::Cfg& themeCfg);

    bool setTheme(const sfw::Theme::Cfg& themeCfg);

    /**
     * Process events from the backend (i.e. SFML)
     */
    void process(const sf::Event& event);

    /**
     * Draw the entire GUI to the backend (i.e. SFML)
     */
    void render();

    /**
     * Change the mouse pointer for the GUI window
     */
    void setMouseCursor(sf::Cursor::Type cursorType);

private:
    /**
     * Get mouse cursor relative position
     * @param x: absolute x position from the event
     * @param y: absolute y position from the event
     * @return relative mouse position
     */
    sf::Vector2f convertMousePosition(int x, int y) const;

    /**
     * Keep a registry of widgets for name->widget lookups (later!) & diagnostics
     */
    friend class Layout;
    void remember(const std::string& name, const Widget* widget);

    sf::RenderWindow& m_window;
    sfw::Theme::Cfg m_themeCfg;

    sf::Cursor::Type m_cursorType;

    std::map<std::string, const Widget*> widgets;
};

} // namespace

#endif // GUI_MAIN_HPP
