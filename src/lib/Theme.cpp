#include "sfw/Theme.hpp"

namespace sfw
{

//!!THESE ARE NOT YET ACTUALLY PACKAGED, but the code is already there, so...:
const char* Theme::Cfg::DEFAULT_basePath = "asset/";
const char* Theme::Cfg::DEFAULT_fontPath = "Vera.ttf";
const char* Theme::Cfg::DEFAULT_texturePath = "texture-baseline.png";

size_t      Theme::Cfg::DEFAULT_textSize = 12;
sf::Color   Theme::Cfg::DEFAULT_bgColor = sf::Color::Black;

bool Theme::Cfg::apply() const
{
    if (!sfw::Theme::loadFont(std::string(DEFAULT_basePath) + (!fontPath ? DEFAULT_fontPath : fontPath)))
    {
        return false; // SFML has already explained the situation...
    }
    if (!texturePath || !Theme::loadTexture(std::string(DEFAULT_basePath) + (!texturePath ? DEFAULT_texturePath : texturePath)))
    {
        return false; // SFML has already explained the situation...
    }
    Theme::windowBgColor = bgColor;
    Theme::textSize = (textSize > 0 ? textSize : DEFAULT_textSize); // Set after loadTexture, as it would reset this.
    return true;
}


static sf::Cursor& getDefaultCursor()
{
    static sf::Cursor cursor;
    if (!cursor.loadFromSystem(sf::Cursor::Arrow))
    {
        //!! loadFromSystem is `nodiscard`...
    }
    return cursor;
}

size_t Theme::textSize = Theme::Cfg::DEFAULT_textSize;
Theme::Style Theme::click;
Theme::Style Theme::input;
sf::Color Theme::windowBgColor = sf::Color::White;
bool Theme::clearWindow = true;
int Theme::borderSize = 1;
int Theme::minWidgetWidth = 86;
float Theme::PADDING = 1.f;
float Theme::MARGIN = 7.f;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
// Note: desig. init would zero the missing fields, which is fine for us, but not for GCC. :-/
sf::Event::KeyEvent Theme::nextWidgetKey =     { .code = sf::Keyboard::Tab };
sf::Event::KeyEvent Theme::previousWidgetKey = { .code = sf::Keyboard::Tab, .shift = true };
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

sf::Font Theme::m_font;
sf::Texture Theme::m_texture;
sf::IntRect Theme::m_subrects[_TEXTURE_ID_COUNT];

sf::Cursor& Theme::cursor = getDefaultCursor();

bool Theme::loadFont(const std::string& filename)
{
    return m_font.loadFromFile(filename);
}


bool Theme::loadTexture(const std::string& filename)
{
    if (m_texture.loadFromFile(filename))
    {
        sf::IntRect subrect;
        subrect.width = m_texture.getSize().x;
        subrect.height = m_texture.getSize().y / _TEXTURE_ID_COUNT;

        for (int i = 0; i < _TEXTURE_ID_COUNT; ++i)
        {
            m_subrects[i] = subrect;
            subrect.top += subrect.height;
        }

        borderSize = subrect.width / 3;
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


const sf::IntRect& Theme::getTextureRect(Box::Type type, WidgetState state)
{
    TextureID id(BOX_DEFAULT);
    switch (state)
    {
    case WidgetState::Default:
        id = type == Box::Click ? BOX_DEFAULT : BOX_INPUT_DEFAULT;
        break;
    case WidgetState::Hovered:
        id = type == Box::Click ? BOX_HOVERED : BOX_INPUT_DEFAULT;
        break;
    case WidgetState::Pressed:
        id = type == Box::Click ? BOX_PRESSED : BOX_INPUT_FOCUSED;
        break;
    case WidgetState::Focused:
        id = type == Box::Click ? BOX_FOCUSED : BOX_INPUT_FOCUSED;
        break;
    }
    return m_subrects[id];
}


const sf::IntRect& Theme::getCheckMarkTextureRect()
{
    return m_subrects[CHECKMARK];
}


const sf::IntRect& Theme::getArrowTextureRect()
{
    return m_subrects[ARROW];
}


const sf::IntRect& Theme::getProgressBarTextureRect()
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

} // namespace
