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

    void process(const sf::Event& event);
    void render();

    /**
     * Update the cursor type on the RenderWindow
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
     * Keep track of widgets, for convenience & diagnostics
     */
    friend class Layout;
    bool remember(const std::string& name, const Widget* widget);

    sf::RenderWindow& m_window;
    sfw::Theme::Cfg m_themeCfg;

    sf::Cursor::Type m_cursorType; // Current mouse pointer shape
    std::map<std::string, const Widget*> widgets;
};

} // namespace

#endif // GUI_MAIN_HPP
