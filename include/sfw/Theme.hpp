#ifndef _GUI_THEME_HPP_
#define _GUI_THEME_HPP_

#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/Gfx/Elements/Wallpaper.hpp"

//!!#include "sfw/Widgets/Tooltip/Style.hpp"

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>

#include <map>
#include <string>

namespace sfw {

class Theme
{
public:

	// Quick-And-Dirty Theme Config Pack
	// -- will gradually encompass the entire Theme class,
	//    by moving from static Theme data to config objects...
	//    This is the initial version to support the Demo app.
	struct Cfg
	{
		// Note: the ordering is optimized for init lists like
		//
		//  { "asset/custom", "MyClassic", "mytextures.png", Color("#e6e8e0"), 12, "font/MyFont.ttf" }
		//
		// Note: these are not "defaults" here, but "unset" markers (and/or diagnostic sentinels)
		// (except when the type doesn't allow such "off-band" values; then it's indeed a default).
		const char* name = nullptr;
		const char* basePath = nullptr;
		const char* textureFile = nullptr;
		sf::Color bgColor = sf::Color::White; // no "unset" color, so must use a sensible default here
		Wallpaper::Cfg wallpaper = {};
		size_t textSize = 0;
		const char* fontFile = nullptr;
		bool multiTooltips = false;

	protected:
		friend class GUI;
		bool apply();
	};

	// The default configuration (see Theme.cpp for details!)
	static Cfg DEFAULT;

	// The current configuration -- !!NOT YET USED (except for the tooltips), BUT WILL REPLACE THE FLAT LEGACTY PARAMS IN THE FUTURE!
	static Cfg cfg;

	// Load the GUI global font
	static bool loadFont(const std::string& path);
	static const sf::Font& getFont();

	// Load & access (parts of) the GUI spritesheet
	static bool loadTexture(const std::string& path);
	static const sf::Texture& getTexture();
	static const sf::IntRect& getTextureRect(Box::Type type, WidgetState state);

		// Convenience helpers:
		static const sf::IntRect& getCheckMarkTextureRect();
		static const sf::IntRect& getArrowTextureRect();
		static const sf::IntRect& getProgressBarTextureRect();

	// Default widget height based on text size
	static float getBoxHeight();

	// Height of (a line of) text
	static int getLineSpacing();

	static sf::Cursor& getDefaultCursor();

	// Two different (input) widget styles: textual ('Input') and generic ('Click')
	//!!CLEANUP! -> #308, #11... Should be more refined & flexible!
	struct Style
	{
		sf::Color textColor;
		sf::Color textColorHover;
		sf::Color textColorFocus;
		sf::Color textColorDisabled;
		sf::Color textSelectionColor;
		sf::Color textPlaceholderColor;
	};
	static Style click;
	static Style input;

	static size_t textSize;

	static sf::Color bgColor;
	static Wallpaper::Cfg wallpaper;
	static bool clearBackground;

	static int borderSize; // Recalculated from the actual texture, so don't try setting it directly...
	static int minWidgetWidth;

	static float PADDING; // Spacing inside widget
	static float MARGIN;  // Spacing between widgets

	static sf::Event::KeyEvent previousWidgetKey;
	static sf::Event::KeyEvent nextWidgetKey;

	// Auto-initialized to default cursor
	static sf::Cursor& cursor;

private:
	enum TextureID
	{
		BOX_DEFAULT,
		BOX_HOVERED,
		BOX_PRESSED,
		BOX_FOCUSED,
		BOX_INPUT_DEFAULT,
		BOX_INPUT_FOCUSED,
		CHECKMARK,
		ARROW,
		PROGRESS_BAR,
		_TEXTURE_ID_COUNT
	};

	static sf::Font m_font;
	static sf::Texture m_texture;
	static sf::IntRect m_subrects[_TEXTURE_ID_COUNT];
}; // class

} // namsepace

#endif // _GUI_THEME_HPP_
