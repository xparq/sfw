#ifndef _KUNYWET5GX94358T645N345TF678RWE5TG67F4B57_
#define _KUNYWET5GX94358T645N345TF678RWE5TG67F4B57_

#include "sfw/Theme.hpp"
#include "sfw/Widget.hpp"
#include "sfw/WidgetPtr.hpp"
#include "sfw/layout/VBox.hpp"
#include "sfw/gfx/element/Wallpaper.hpp"
#include "sfw/gfx/Color.hpp"
#include "sfw/gfx/Render.hpp"
#include "sfw/math/Vector.hpp"
#include "sfw/Event.hpp"

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

	bool setTheme(const sfw::Theme::Cfg& themeCfg);

	/**
	 * Broadcast an onThemeChanged notification to all widgets, so they
	 * can react to changes made directly to the current (static) theme
	 * rather than having an entirely new theme selected by setTheme().
	 *
	 *!! Temp./interim hack until having Theme::set(prop, value), or at
	 *!! least Theme::notify(event), doing the notification implicitly!
	 */
	void themeChanged();

	/**
	 * Overloaded Widget::setPosition() for wallpaper support (etc.)
	 */
	void setPosition(const fVec2& pos);
	void setPosition(float x, float y);

	//!!bool setSize(fVec2 size);
	//!! For now, only the inherited version is available, which isn't aware of
	//!! the case of managing a host window

	/**
	 * Current GUI rectangle size: if the GUI manages (owns) the (host) window,
	 * then the size of that, otherwise the current size the rectangle the GUI
	 * actually occupies
	 */
	fVec2 getSize() const;

	/**
	 * Returns true if no errors & the GUI has not been closed or deactivated.
	 */
	bool active();
	operator bool() { return active(); }


	/**
	 * Get the last input event from the active input event source
	 * (The input source is always the event queue of the associated window
	 * currently.)
	 */
	event::Input poll();

	/**
	 * Process input event
	 * Returns false if the GUI is inactive, or when the underlying backend's
	 * CLOSE event has been received. In that case close() will also be called,
	 * which will deactivate the GUI (but not close the host window itself,
	 * unless the GUI was set up to manage ("own") the window).
	 */
	bool process(const event::Input& event);

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
	bool remember(Widget* widget, std::string_view name = "", bool override_existing = true);
	Widget* recall(std::string_view name) const;
	std::string recall(const Widget*) const;

	// Get mouse cursor position in coordinates local to the main GUI window
	fVec2 getMousePosition() const;

	/**
	 * Set/manage wallpaper image
	 * If `filename` is omitted, the wallpaper configured for the current theme
	 * (if any) will be used.
	 */
	void setWallpaper(const Wallpaper::Cfg& cfg);
	void setWallpaper(std::string filename = "",
					  Wallpaper::Placement placement = sfw::Wallpaper::Placement::Center,
					  Color tintColor = Color::White);
	const Wallpaper& getWallpaper() const { return m_wallpaper; }
	void disableWallpaper();
	bool hasWallpaper();
	/**
	 * Wallpaper color "tint" and/or transparency (alpha) level
	 */
	void  setWallpaperColor(Color tint);
	Color getWallpaperColor() const;

	// Accumulated session time (minus while !active()), in seconds
	float sessionTime() const;

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

	// Convert screen pixel coordinates to window (canvas) position (See SFML's sf::Mouse docs!)
	fVec2 convertMousePosition(iVec2 mouse_pos)          const;
	fVec2 convertMousePosition(int mouse_x, int mouse_y) const { return convertMousePosition({mouse_x, mouse_y});}

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
	sf::RenderWindow& m_window;      //!! Refine to focus on being a RenderTarget (the host windowing chores could have a separate API)
	sfw::event::Source m_inputQueue; //!! ...especially as the event handling has now been abstracted away from the window!
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
