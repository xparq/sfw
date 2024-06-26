#include "sfw/Theme.hpp"
#include "sfw/util/diagnostics.hpp"

#include <string>
	using std::string;

namespace sfw
{
using namespace geometry;

Theme::Cfg Theme::DEFAULT =
{
	.name = "",
	.basePath = "asset/",
	.textureFile = "texture/default.png",
	.bgColor = sf::Color::White,
	.wallpaper = {},
	.textSize = 12,
	.fontFile = "font/default.ttf",
	.multiTooltips = false, //! Ignotred! The desig. init. value can't be made distinguishable
	                        //! from an artificial "use default" inline member-init val. in Cfg::.
};


bool Theme::Cfg::apply()
{
	// Pick up some defaults first...
	// (This really should be a constructor, but that would piss off C++ and make it reject the designated inits. :-/ )
	if (!name)         name        = Theme::DEFAULT.name;
	if (!basePath)     basePath    = Theme::DEFAULT.basePath;
	if (!fontFile)     fontFile    = Theme::DEFAULT.fontFile;
	if (!textureFile)  textureFile = Theme::DEFAULT.textureFile;
	if (textSize <= 1) textSize    = Theme::DEFAULT.textSize;

	// Update the global (`static`) Theme data...

	// Save the config params first, for later reference... (!!Should later become the full, normative theme config itself!)
	Theme::cfg = *this;

	if (!Theme::loadFont(string(basePath) + fontFile))
	{
		return false; // SFML has already explained the situation...
	}
	if (!Theme::loadTexture(string(basePath) + textureFile))
	{
		return false; // SFML has already explained the situation...
	}
	Theme::textSize = textSize;

	Theme::bgColor = bgColor;
	Theme::wallpaper = wallpaper;

	return true;
}

//============================================================================
Theme::Cfg Theme::cfg; //!! Mostly unused yet, but slowly migrating to it...

size_t Theme::textSize = Theme::DEFAULT.textSize;
Theme::Style Theme::click;
Theme::Style Theme::input;

sf::Color Theme::bgColor = sf::Color::White;
Wallpaper::Cfg Theme::wallpaper{};
bool Theme::clearBackground = true;

int Theme::borderSize = 1; //! Will get reset based on the loaded texture, so no use setting it here!
int Theme::minWidgetWidth = 86;
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

sf::Font Theme::m_font;
/*!!sfw::gfx::!!*/Texture Theme::m_texture;
iRect Theme::m_subrects[_TEXTURE_ID_COUNT];
sf::Cursor& Theme::cursor = getDefaultCursor();


//============================================================================
sf::Cursor& Theme::getDefaultCursor()
{
	static sf::Cursor c; //!! Basically a singleton system resource anyway, and
	                     //!! Theme is also static, so no "additional" harm done...
	                     //!! But if it does support RAII with stacked state restore,
	                     //!! then this should not be static!
	if (!c.loadFromSystem(sf::Cursor::Type::Arrow))
	{
		//!! loadFromSystem is `nodiscard`...
	}
	return c;
}


bool Theme::loadFont(const std::string& filename)
{
	return m_font.loadFromFile(filename);
}


bool Theme::loadTexture(const std::string& filename)
{
	if (m_texture.load(filename))
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


const sf::Font& Theme::getFont()
{
	return m_font;
}


const sf::Texture& Theme::getTexture()
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


int Theme::getLineSpacing()
{
	return (int)m_font.getLineSpacing((unsigned)textSize);
}

} // namespace sfw
