#ifndef _SUIOHTCY78Y678E4785T36T6B67X34574NY7T_
#define _SUIOHTCY78Y678E4785T36T6B67X34574NY7T_


#include "sfw/gfx/element/Texture.hpp"
#include "sfw/gfx/element/Font.hpp"
#include "sfw/gfx/element/Box.hpp"
#include "sfw/gfx/element/Wallpaper.hpp"
//!! Make it modular, so each component can have its own theming definitions:
//!!#include "sfw/widget/Tooltip/Style.hpp"

//#include "SAL/geometry/Rectangle.hpp" // Definitely done already by the rectangular things above. ;)
//#include "sfw/gfx/Color.hpp"          // Also this, by those colorful things above. :)

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <map>
#include <string>
#include <string_view>


namespace sfw {

class Theme
{
public:

	// Quick-And-Dirty Theme Config Pack
	// -- will gradually encompass the entire Theme class,
	//    by moving from static Theme data to config objects...
	//    This is the initial version, just to support the demo/test apps.
	struct Cfg
	{
		// Note: the ordering is optimized for init lists like
		//
		//  { "asset_dir/custom", "MyTheme", "mytextures.png", Color("#e6e8e0"), 12, "font/MyFont.ttf" }
		//  { "asset_dir/custom", "MyTheme", "mytextures.png", Color("#e6e8e0"), 14 } // Default font, but larger
		//
		// Note: these are not "defaults" here, but "unset" markers (and/or diagnostic sentinels)
		// (except when the type doesn't allow such "off-band" values; then it's indeed a default).
		const char*    name          = nullptr;
		const char*    basePath      = nullptr;
		const char*    textureFile   = nullptr;
		Color          bgColor       = 0xBadC0100; // Not using (reserving!) Color::None here as the unset ("null") value,
		                                           // to allow using it for "no background color" in a theme config!
		                                           // See #425 for (much ;) ) more!
		Wallpaper::Cfg wallpaper     = {};
		size_t         textSize      = 0;          //!! #424 (Rename), #426 (Stop using size_t for trivially small things)
		const char*    fontFile      = nullptr;
		bool           multiTooltips = false;

	protected:
		friend class GUI;
		bool apply();
	};

	// The default configuration (see Theme.cpp for details!)
	static Cfg DEFAULT;

	// The current configuration -- !!NOT YET USED (except for the tooltips), BUT WILL REPLACE THE FLAT LEGACTY PARAMS IN THE FUTURE!
	static Cfg cfg;

	// Load the GUI global font
	static bool loadFont(std::string_view path);
	static const gfx::Font& getFont();

	// Load & access (parts of) the GUI spritesheet
	static bool loadTexture(std::string_view path);
	static const gfx::Texture& getTexture();
	static const geometry::iRect& getTextureRect(Box::Type type, ActivationState state);

		// Convenience helpers:
		static const geometry::iRect& getCheckMarkTextureRect();
		static const geometry::iRect& getArrowTextureRect();
		static const geometry::iRect& getProgressBarTextureRect();

	// Default widget height based on text size
	static float getBoxHeight();

	// Height of a line of text with the current base text size
	static float getLineSpacing();

	static const sf::Cursor& getDefaultMousePointer();

	// Two different (input) widget styles: textual ('Input') and generic ('Click')
	//!!CLEANUP! -> #308, #11... Should be more refined & flexible!
	struct Style
	{
		Color textColor;
		Color textColorHover;
		Color textColorFocus;
		Color textColorDisabled;
		Color textSelectionColor;
		Color textPlaceholderColor;
	};
	static Style click;
	static Style input;

	static size_t textSize;     //!! #424 (Rename), #426 (Stop using size_t for trivially small things)

	static Color bgColor;
	static Wallpaper::Cfg wallpaper;
	static bool clearBackground;

	static int borderSize; // Recalculated from the actual texture, so don't try setting it directly...
	static int minWidgetWidth;

	static float PADDING; // Spacing inside widgets
	static float MARGIN;  // Spacing between widgets

	static sf::Event::KeyChanged previousWidgetKey;
	static sf::Event::KeyChanged nextWidgetKey;

	static const sf::Cursor& mousePointer;

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

	static gfx::Font m_font;
	static gfx::Texture m_texture;
	static geometry::iRect m_subrects[_TEXTURE_ID_COUNT];
}; // class

} // namsepace sfw


#endif // _SUIOHTCY78Y678E4785T36T6B67X34574NY7T_
