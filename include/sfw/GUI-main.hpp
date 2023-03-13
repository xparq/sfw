#ifndef GUI_MAIN_HPP
#define GUI_MAIN_HPP

#include "sfw/Theme.hpp"
#include "sfw/Gfx/Render.hpp"
#include "sfw/Layouts/VBox.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Cursor.hpp>

#include <string>
#include <map>
#include <system_error>

namespace sfw
{

/**
 * Entry point for the GUI.
 * The Main object is actually a VBox, with a catch-all event handler
 * and a bunch of "administrative" duties.
 */
class GUI: public VBox
{
public:
    GUI(sf::RenderWindow& window, const sfw::Theme::Cfg& themeCfg = Theme::DEFAULT);

    /**
     * Return true if no errors.
     */
    operator bool();


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
     * Implement a name->widget registry
     */
    void remember(const std::string& name, Widget* widget);
    Widget* recall(const std::string& name) const;
    std::string recall(const Widget*) const;

    /**
     * Change the mouse pointer for the GUI window
     */
    void setMouseCursor(sf::Cursor::Type cursorType);

private:

    /**
     * "Soft-reset" the GUI state, keeping the current config & widgets
     * Clears the error state, too.
     * NOTE: It would fall back to erring if the config itself is invalid!
     * @return true if no errors
     */
    bool reset();

    /**
     * Get mouse cursor relative position
     * @param x: absolute x position from the event
     * @param y: absolute y position from the event
     * @return relative mouse position
     */
    sf::Vector2f convertMousePosition(int x, int y) const;

    std::error_code m_error;
    sf::RenderWindow& m_window;
    sfw::Theme::Cfg m_themeCfg;
    sf::Cursor::Type m_cursorType;
    std::map<std::string, Widget*> widgets;

#ifdef DEBUG
    friend class Layout;
#endif
};

} // namespace

#endif // GUI_MAIN_HPP
