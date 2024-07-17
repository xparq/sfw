#include "sfw/Theme.hpp"

#include "SAL/util/diagnostics.hpp"

#include <string>
	using std::string;
#include <string_view>
	using std::string_view;


namespace sfw
{
using namespace geometry;

//============================================================================
// BEGIN class Theme::Cfg
//============================================================================

bool Theme::Cfg::apply()
{
	// Pick up some defaults first...
	// (This really should be a constructor, but that would piss off C++, making it reject desig. init.! :-/ )
	if (!name)                 name        = Theme::DEFAULT.name;
	if (!basePath)             basePath    = Theme::DEFAULT.basePath;
	if (!fontFile)             fontFile    = Theme::DEFAULT.fontFile;
	if (!textureFile)          textureFile = Theme::DEFAULT.textureFile;
	if (fontSize <= 1)         fontSize    = Theme::DEFAULT.fontSize;
	if (bgColor == 0xBadC0100) bgColor     = Theme::DEFAULT.bgColor; // See #425 for that code choice!

	// Update the global (`static`) Theme data...

	// Save the config params first, for later reference... (!!Should later become the full, normative theme config itself!)
	Theme::cfg = *this;

	if (string path = string(basePath) + fontFile; !Theme::loadFont(path))
	{
		return false; //!! SFML has already explained the situation...
	}
	if (string path = string(basePath) + textureFile; !Theme::loadTexture(path))
	{
		return false; //!! SFML has already explained the situation...
	}
	Theme::fontSize = fontSize;

	Theme::bgColor = bgColor;
	Theme::wallpaper = wallpaper;

	return true;
}

//============================================================================
// END class Theme::Cfg
//============================================================================


//============================================================================
// BEGIN class Theme
//============================================================================

Theme::Cfg Theme::DEFAULT =
{
	.name          = "",
	.basePath      = "asset/",
	.textureFile   = "texture/default.png",
	.bgColor       = Color::White,
	.wallpaper     = {},
	.fontSize      = 12,
	.fontFile      = "font/default.ttf",
	.multiTooltips = false, //! Ignored! The desig. init. value can't be made distinguishable
	                        //! from an artificial "use default" inline member-init val. in Cfg::.
};

Theme::Cfg Theme::cfg; //!! Mostly unused yet, but slowly migrating to it...

unsigned Theme::fontSize = Theme::DEFAULT.fontSize;

//!! LEGACY:
Theme::Style Theme::click;
Theme::Style Theme::input;

Color          Theme::bgColor = Color::White;
Wallpaper::Cfg Theme::wallpaper{};
bool           Theme::clearBackground = true;

//!! CONSOLIDATE:
int   Theme::borderSize = 1; //! Will get reset based on the loaded texture, so no use setting it here!
int   Theme::minWidgetWidth = 86;
float Theme::PADDING = 1.f;
float Theme::MARGIN = 4.f;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
// Note: desig. init would zero the missing fields, which is fine for us, but not for GCC. :-/
#endif
sf::Event::KeyChanged Theme::nextWidgetKey =     { .code = sf::Keyboard::Key::Tab };
sf::Event::KeyChanged Theme::previousWidgetKey = { .code = sf::Keyboard::Key::Tab, .shift = true };
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

gfx::Font     Theme::m_font;
gfx::Texture  Theme::m_texture;
iRect                 Theme::m_subrects[_TEXTURE_ID_COUNT];

const sf::Cursor&     Theme::mousePointer = getDefaultMousePointer();


//============================================================================
const sf::Cursor& Theme::getDefaultMousePointer()
{
	//! Creates and copies a whole new cursor instance on every call! :-/
	//! (Fortuantely, it's only even called once, though...)

	static const auto c = sf::Cursor::loadFromSystem(sf::Cursor::Type::Arrow); // std::optional, actually
		//!! Basically a singleton system resource anyway, and
	        //!! Theme is also static, so no "additional" harm done...
	        //!! But if it does support RAII with stacked state restore,
	        //!! then this should not be static!
	if (!c)
	{
		//!! Hopefully even a failed load...() will at least keep the default!... :)
		throw ("Failed to set the default mouse pointer!"); //!!?? WTF...
		//!! [[unreachable]]
	}
	return c.value();
}


bool Theme::loadFont(string_view path)
{
	return m_font.load(path);
}


bool Theme::loadTexture(string_view path)
{
	if (m_texture.load(path))
	{
		iRect subrect;
		//!! Alas, rect.width()/height() are NOT lvalues; need to use the alt. setter API:
		subrect.width(m_texture.size().x());
		subrect.height(m_texture.size().y() / _TEXTURE_ID_COUNT);

		for (int i = 0; i < _TEXTURE_ID_COUNT; ++i)
		{
			m_subrects[i] = subrect;
			subrect.top() += subrect.height();
		}

		borderSize = subrect.width() / 3;
		return true;
	}
	return false;
}


const gfx::Font& Theme::getFont()
{
	return m_font;
}


const gfx::Texture& Theme::getTexture()
{
	return m_texture;
}


const iRect& Theme::getTextureRect(Box::Type type, ActivationState state)
{
	TextureID id(BOX_DEFAULT);
	switch (state)
	{
	case ActivationState::Default:
		id = type == Box::Click ? BOX_DEFAULT : BOX_INPUT_DEFAULT;
		break;
	case ActivationState::Hovered:
		id = type == Box::Click ? BOX_HOVERED : BOX_INPUT_DEFAULT;
		break;
	case ActivationState::Pressed:
		id = type == Box::Click ? BOX_PRESSED : BOX_INPUT_FOCUSED;
		break;
	case ActivationState::Focused:
		id = type == Box::Click ? BOX_FOCUSED : BOX_INPUT_FOCUSED;
		break;
	case ActivationState::Disabled:
		id = type == Box::Click ? BOX_DEFAULT : BOX_INPUT_DEFAULT;
		break;
	}
	return m_subrects[id];
}


const iRect& Theme::getCheckMarkTextureRect()
{
	return m_subrects[CHECKMARK];
}


const iRect& Theme::getArrowTextureRect()
{
	return m_subrects[ARROW];
}


const iRect& Theme::getProgressBarTextureRect()
{
	return m_subrects[PROGRESS_BAR];
}


float Theme::getBoxHeight()
{
	return getLineSpacing() + borderSize * 2 + PADDING * 2;
}


float Theme::getLineSpacing()
{
	return m_font.lineSpacing((unsigned)fontSize);
}


} // namespace sfw
