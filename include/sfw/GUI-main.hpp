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
    GUI(sf::RenderWindow& window, const sfw::Theme::Cfg& themeCfg = Theme::DEFAULT,
        bool own_the_window = true);

    /**
     * Return true if no errors & has not been closed.
     */
    bool active();
    operator bool() { return active(); }

    bool setTheme(const sfw::Theme::Cfg& themeCfg);

    /**
     * Process events from the backend (i.e. SFML)
     * Returns false if the GUI is inactive, or when the underlying backend's
     * CLOSE event has been received. In that case close() will also be called
     * (which will deactivate the GUI).
     */
    bool process(const sf::Event& event);

    /**
     * Draw the entire GUI to the backend (i.e. SFML)
     */
    void render();

    /**
     * Shut down the GUI, and close the window, too, if "owning" it
     */
    void close();

    /*************************************************************************
     Name->widget registry

     If name == "", the widget will be registered with a default name, which
     is also a unique internal ID (for disagnostic purposes).

     If `override_existing` is true, a previously registered widget with the
     same name will lose its name, and will revert to having a default instead.

     Notes:
     - Since default names uniquely identify widgets, the override flag is
       redudndant with empty names.
     - The registry doesn't store const pointers, to allow any widget operations
       to be applied directly on a retrieved pointer.
     - If you're wondering why remember() doesn't have a const string& input
       type, see its implementation.
     *************************************************************************/
    bool remember(Widget* widget, std::string name, bool override_existing = true);
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
    bool m_own_the_window;
    sfw::Theme::Cfg m_themeCfg;
    sf::Cursor::Type m_cursorType;
    std::map<std::string, Widget*> widgets;

    bool m_closed = false;
};

} // namespace

#endif // GUI_MAIN_HPP
