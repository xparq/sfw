#ifndef GUI_TEXTBOX_HPP
#define GUI_TEXTBOX_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Shapes/Box.hpp"

#include <string>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Clock.hpp>

namespace sfw
{

/**
 * Generalized text selection abstraction, independent from the internals
 * of the text editor using it. The editor should hook up various selection
 * methods in its event handlers and operations (most importantly `follow(pos)`
 * into its cursor updating proc.) to achieve the desired selection behavior.
 * (This implementation is optimized for volatile selections, but can easily
 * support persistent modes, too... I guess... Never actually tried. ;) )
 */
struct TextSelection
{
    size_t anchor_pos = 0;
    size_t active_pos = 0;
    bool following = false;

    operator bool() const { return !empty(); }
    size_t empty() const { return active_pos == anchor_pos; }
    size_t length() const { return right() - left(); }
    // These're about *appearance*, so should be OK even with right-to-left scripts:
    size_t left() const;
    size_t right() const;

    void start(size_t pos);
    void set(size_t pos, size_t len = 0);
    void set_from_to(size_t anchor_pos, size_t active_pos);
    void resize(size_t len);
    void follow(size_t pos);
    void stop();
    void resume();
    void reset();
};


/**
 * The TextBox widget is a one-line text editor.
 * It allows the user to enter and edit a single line of plain text.
 */
class TextBox: public Widget
{
public:
    enum CursorStyle
    {
        BLINK,
        PULSE
    };

    TextBox(float width = 200.f, CursorStyle style = BLINK);

    /**
     * Define textbox content
     */
    TextBox* setText(const sf::String& string);

    /**
     * Get textbox content
     */
    const sf::String& getText() const;

    /**
     * Define max length of textbox content (default is 256 characters)
     */
    TextBox* setMaxLength(size_t maxLength);

    /**
     * Set the cursor position
     */
    void setCursorPos(size_t index);

    /**
     * Get the cursor position
     */
    size_t getCursorPos() const { return m_cursorPos; }

    /**
     * Set the selection to a specific range
     */
    void setSelection(size_t from, size_t to);

    /**
     * Get selected text
     */
    sf::String getSelectedText() const;

    /**
     * Delete selected text, if any
     */
    void deleteSelectedText();

    /**
     * Cancel the text selection, if any
     */
    void clearSelection();

    /**
     * Set placeholder text
     */
    TextBox* setPlaceholder(const sf::String& placeholder);

    /**
     * Get placeholder text
     */
    const sf::String& getPlaceholder() const;

    TextBox* setCallback(std::function<void()> callback)         { return (TextBox*) Widget::setCallback(callback); }
    TextBox* setCallback(std::function<void(TextBox*)> callback);

private:
    void draw(const gfx::RenderContext& ctx) const override;

    // Callbacks
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onKeyReleased(const sf::Event::KeyEvent& key) override;
    void onMouseEnter() override;
    void onMouseLeave() override;
    void onMousePressed(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onMouseMoved(float x, float y) override;
    void onTextEntered(uint32_t unicode) override;
    void onStateChanged(WidgetState state) override;
    void onThemeChanged() override;

    // Config:
    size_t m_maxLength;
    float m_width;
    CursorStyle m_cursorStyle;
    float m_cursorBlinkPeriod = 1.f;
    // Editor inrernal state:
    sf::Text m_text;
    sf::Text m_placeholder;
    size_t m_cursorPos; // Despite the name, this isn't a property of the visual cursor representation
    TextSelection m_selection;
    // Cursor visual state:
    mutable sf::RectangleShape m_cursorRect;
    mutable sf::Color m_cursorColor;
    mutable sf::Clock m_cursorTimer;
    // Widget state:
    Box m_box;
};

} // namespace

#endif // GUI_TEXTBOX_HPP
