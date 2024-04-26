#ifndef _KUNYWET5GX94358T645N345TF678RWE5TG67F4B57_
#define _KUNYWET5GX94358T645N345TF678RWE5TG67F4B57_

#include "sfw/Theme.hpp"
#include "sfw/Gfx/Render.hpp"
#include "sfw/Widget.hpp"
#include "sfw/WidgetPtr.hpp"
#include "sfw/Layouts/VBox.hpp"
#include "sfw/Gfx/Elements/Wallpaper.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Cursor.hpp>
#include <SFML/System/Clock.hpp>

#include <string_view>
#include <string>
#include <unordered_map>
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
	 * Broadcast the onThemeChanged notification so widgets can pick up any
	 * manual changes made directly to the current (static) theme props.
	 */
	void themeChanged();

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
	 * Shut down the GUI, and close the window, too, if owning it
	 */
	void close();

	bool closed();


	/*************************************************************************
	 Name->widget map (registry)

	 If name == "", the widget will be registered with a default name, which
	 is also a unique internal ID (for disagnostic purposes).

	 If `override_existing` is true, a previously registered widget with the
	 same name will lose its name, and will revert to having an interanl ID
	 instead.

	 Notes:
	 - Since default names uniquely identify widgets, the override flag is
	   redudndant with empty names.
	 - The registry stores non-const widget pointers (to allow any widget
	   operations to be applied directly on a retrieved pointer).
	 *************************************************************************/
	bool remember(Widget* widget, std::string name, bool override_existing = true);
	Widget* recall(std::string_view name) const;
	std::string recall(const Widget*) const;

	/**
	 * Change the mouse pointer for the GUI window
	 */
	void setMouseCursor(sf::Cursor::Type cursorType);

	// Get mouse cursor position (in "GUI Main" coordinates)
	sf::Vector2f getMousePosition() const;

	/**
	 * Set/manage wallpaper image
	 * If `filename` is omitted, the wallpaper configured for the current theme
	 * (if any) will be reset.
	 */
	void setWallpaper(const Wallpaper::Cfg& cfg);
	void setWallpaper(std::string filename = "",
					  Wallpaper::Placement placement = sfw::Wallpaper::Placement::Center,
					  sf::Color tintColor = sf::Color::White);
	const Wallpaper& getWallpaper() const { return m_wallpaper; }
	void disableWallpaper();
	bool hasWallpaper();
	/**
	 * Tint the wallpaper and/or set its transparency (alpha) level
	 */
	void setWallpaperColor(sf::Color tint);

	/**
	 * Overloaded Widget::setPosition() for wallpaper support (etc.)
	 */
	void setPosition(const sf::Vector2f& pos);
	void setPosition(float x, float y);

	/**
	 * Current GUI rectangle size: if the GUI manages (owns) the (host) window,
	 * then the size of that, otherwise the current size the rectangle the GUI
	 * actually occupies
	 */
	sf::Vector2f getSize() const;

	// Accumulated session time (minus while !active()), in seconds
	float sessionTime() const;

private:
	/**
	 * "Soft-reset" the GUI state, keeping the current config & widgets
	 * Clears the error state, too.
	 * NOTE: It would fall back to erring if the config itself is invalid!
	 * @return true if no errors
	 */
	bool reset();

	// Convert screen pixel coordinates to window position (See the sf::Mouse docs!)
	sf::Vector2f convertMousePosition(int x, int y) const;

// ---- Callbacks ------------------------------------------------------------
	void draw(const gfx::RenderContext& ctx) const override;

	// If the GUI doesn't manage the entire window, and its size hasn't been
	// fixed explicitly, then it'll auto-grow as widgets are added (or removed).
	// Certain internal functions may need to keep up with those changes.
	void onResized() override;
	void onTick() override;
	// If the user had changed the theme params. directly, GUI.setTheme() should
	// then be called, too, which would in turn trigger this callback (just like
	// for all the other widgets)
	void onThemeChanged() override;

// ---- Helpers --------------------------------------------------------------

	// Fill the window if managing (owning) it, else just the GUI rect,
	// with the current Theme::bgColor, then show the wallpaper, if set
	// Note: a clearing background fill is needed even when there's a
	// wallpaper, because the image may be translucent!
	void renderBackground();

// ---- Data -----------------------------------------------------------------
	std::error_code m_error;
	sf::RenderWindow& m_window;
	bool m_own_window;
	sfw::Theme::Cfg m_themeCfg;
	sfw::Wallpaper m_wallpaper;
	sf::Cursor::Type m_cursorType;
	sf::Clock m_clock;
	sf::Time m_sessionTime;
	std::unordered_map<std::string, Widget*> widgets;
	bool m_closed = false;

// ---- Misc. hackery... -----------------------------------------------------
	// Convenience helper to find the GUI instance easily (assuming the client
	// app only has one). Init'ed/updated by the GUI::ctor.
	static GUI* DefaultInstance;
	// And allow (only) the Widget class to actually use it:
	friend class Widget;
};

} // namespace sfw

#endif // _KUNYWET5GX94358T645N345TF678RWE5TG67F4B57_
