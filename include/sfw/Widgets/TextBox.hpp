#ifndef GUI_TEXTBOX_HPP
#define GUI_TEXTBOX_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/Gfx/Elements/Text.hpp"
#include "sfw/TextSelection.hpp"

#include <string>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Clock.hpp>

namespace sfw
{

/*****************************************************************************
  The TextBox widget is a one-line plain-text editor, with selection support.
 *****************************************************************************/
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
     * Set textbox content
     */
    TextBox* setText(const sf::String& string);

    /**
     * Get textbox content
     */
    const sf::String& getText() const;

    size_t getTextLength() const { return m_text.getString().getSize(); }

    /**
     * Set max length of textbox content (default is 256 characters)
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
    void onTextEntered(char32_t unichar) override;
    void onStateChanged(WidgetState state) override;
    void onThemeChanged() override;

    // Config:
    size_t m_maxLength;
    float m_width;
    CursorStyle m_cursorStyle;
    float m_cursorBlinkPeriod = 1.f;
    // Editor inrernal state:
    Text m_text;
    Text m_placeholder;
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
