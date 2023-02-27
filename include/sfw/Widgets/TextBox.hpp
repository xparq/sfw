#ifndef GUI_TEXTBOX_HPP
#define GUI_TEXTBOX_HPP

#include "sfw/Widget.hpp"
#include "sfw/Shapes/Box.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>

namespace gui
{

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
    void setText(const sf::String& string);

    /**
     * Get textbox content
     */
    const sf::String& getText() const;

    /**
     * Define max length of textbox content (default is 256 characters)
     */
    void setMaxLength(size_t maxLength);

    /**
     * Set the cursor position
     */
    void setCursor(size_t index, bool extend_selection = false);

    /**
     * Get the cursor position
     */
    size_t getCursor() const;

    /**
     * Set selected text
     */
    void setSelectedText(size_t from, size_t to);

    /**
     * Get selected text
     */
    const sf::String& getSelectedText() const;

    /**
     * Cancel the text selection range, if any
     */
    void clearSelectedText();

    /**
     * Set placeholder text
     */
    void setPlaceholder(const sf::String& placeholder);

    /**
     * Get placeholder text
     */
    const sf::String& getPlaceholder() const;

protected:
    // Callbacks
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onMouseEnter() override;
    void onMouseLeave() override;
    void onMousePressed(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onMouseMoved(float x, float y) override;
    void onTextEntered(uint32_t unicode) override;
    void onStateChanged(WidgetState state) override;

private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    /**
     * Delete selected text if any
     */
    void deleteSelectedText();

    sf::Text m_text;
    sf::Text m_placeholder;
    Box m_box;
    CursorStyle m_cursorStyle;
    mutable sf::RectangleShape m_cursor;
    mutable sf::Clock m_cursorTimer;
    size_t m_cursorPos;
    size_t m_maxLength;
    size_t m_selectionFirst;
    size_t m_selectionLast;
    sf::String m_selectedText;
};

}

#endif // GUI_TEXTBOX_HPP
