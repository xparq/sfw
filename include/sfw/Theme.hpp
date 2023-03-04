#ifndef GUI_THEME_HPP
#define GUI_THEME_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Shapes/Box.hpp"

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>

#include <map>
#include <string>

namespace sfw
{
class Theme
{
public:

    // Quick-And-Dirty Theme Config Pack
    // -- will gradually encompass the entire Theme class,
    //    by moving from static Theme data to config objects...
    //    This is the initial version to support the Demo app.
    struct Cfg
    {
        //!! These make no real sense until they get actually packaged:
        static const char* DEFAULT_basePath; // "asset/"
        static const char* DEFAULT_fontPath; // Bitstream Vera
        static const char* DEFAULT_texturePath; 

        static size_t      DEFAULT_textSize;
        static sf::Color   DEFAULT_bgColor;

        const char* name = "(unnamed)";
        const char* texturePath = nullptr;
        sf::Color bgColor = DEFAULT_bgColor;
        size_t textSize = DEFAULT_textSize;
        const char* fontPath = nullptr;

    protected:
        friend class GUI;
	bool apply() const;
    };


    /**
     * Load the GUI global font
     */
    static bool loadFont(const std::string& path);
    static const sf::Font& getFont();

    /**
     * Load the GUI spritesheet
     */
    static bool loadTexture(const std::string& path);
    static const sf::Texture& getTexture();

    static const sf::IntRect& getTextureRect(Box::Type type, WidgetState state);

    static const sf::IntRect& getCheckMarkTextureRect();
    static const sf::IntRect& getArrowTextureRect();
    static const sf::IntRect& getProgressBarTextureRect();

    /**
     * Default widget height based on text size
     */
    static float getBoxHeight();

    /**
     * Height of a line of text
     */
    static int getLineSpacing();

    static size_t textSize;
    struct Style
    {
        sf::Color textColor;
        sf::Color textColorHover;
        sf::Color textColorFocus;
        sf::Color textSelectionColor;
        sf::Color textPlaceholderColor;
    };

    static Style click;
    static Style input;

    static sf::Color windowBgColor;
    static bool clearWindow;
    static int borderSize; // Recalculated by applying a new theme (so no use setting it usually)
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
};

} // namsepace

#endif // GUI_THEME_HPP
