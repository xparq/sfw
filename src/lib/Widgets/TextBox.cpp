#include "sfw/Widgets/TextBox.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/OpenGL.hpp>

#include <functional>

//#include <iostream> //!DEBUG
//using namespace std;

#define BLINK_PERIOD 1.f

namespace sfw
{

TextBox::TextBox(float width, CursorStyle style):
    m_box(Box::Input),
    m_cursorStyle(style),
    m_cursorPos(0),
    m_maxLength(256),
    m_selectionFirst(0),
    m_selectionLast(0)
{
    m_box.setSize(width, Theme::getBoxHeight());

    float offset = Theme::borderSize + Theme::PADDING;
    m_text.setFont(Theme::getFont());
    m_text.setPosition({offset, offset});
    m_text.setFillColor(Theme::input.textColor);
    m_text.setCharacterSize((unsigned)Theme::textSize);

    m_placeholder.setFont(Theme::getFont());
    m_placeholder.setPosition({offset, offset});
    m_placeholder.setFillColor(Theme::input.textPlaceholderColor);
    m_placeholder.setCharacterSize((unsigned)Theme::textSize);

    m_cursor.setPosition({offset, offset});
    m_cursor.setSize(sf::Vector2f(1.f, (float)Theme::getLineSpacing()));
    m_cursor.setFillColor(Theme::input.textColor);
    setCursor(0);

    setSize(m_box.getSize());
}


TextBox* TextBox::setText(const sf::String& string)
{
    m_text.setString(string.substring(0, m_maxLength)); // Limit the length
    setCursor(getText().getSize());

    return this;
}


const sf::String& TextBox::getText() const
{
    return m_text.getString();
}


TextBox* TextBox::setMaxLength(size_t maxLength)
{
    m_maxLength = maxLength;
    // Trim current text if needed
    if (m_text.getString().getSize() > m_maxLength)
    {
        m_text.setString(m_text.getString().substring(0, m_maxLength));
        setCursor(m_maxLength);
    }

    return this;
}


void TextBox::setCursor(size_t index, bool extend_selection)
{
    if (index <= m_text.getString().getSize())
    {
        m_cursorPos = index;
        if (extend_selection)
        {
            // Alas, can't just preset m_selectionFirst/Last directly, because
            // setSelectedText() would then think: nothing to do. Must use copies:
            auto selfirst = m_selectionFirst;
            auto sellast  = m_selectionLast;
            //!!This is a crude quick addition only for #51 yet, that only works *sometimes*!
            //!!(E.g. it can only extend, but not shrink; etc.)
            if (index < selfirst) selfirst = index;
            if (index > sellast)  sellast  = index;
//cerr << selfirst << " - " << sellast << endl;
            setSelectedText(selfirst, sellast);
        }
        else
        {
            m_selectionFirst = index;
            m_selectionLast = index;
        }

        float padding = Theme::borderSize + Theme::PADDING;
        m_cursor.setPosition({m_text.findCharacterPos(index).x, padding});
        m_cursorTimer.restart();

        if (m_cursor.getPosition().x > getSize().x - padding)
        {
            // Shift text on left
            float diff = m_cursor.getPosition().x - getSize().x + padding;
            m_text.move({-diff, 0});
            m_cursor.move({-diff, 0});
        }
        else if (m_cursor.getPosition().x < padding)
        {
            // Shift text on right
            float diff = padding - m_cursor.getPosition().x;
            m_text.move({diff, 0});
            m_cursor.move({diff, 0});
        }

        float textWidth = m_text.getLocalBounds().width;
        if (m_text.getPosition().x < padding && m_text.getPosition().x + textWidth < getSize().x - padding)
        {
            float diff = (getSize().x - padding) - (m_text.getPosition().x + textWidth);
            m_text.move({diff, 0});
            m_cursor.move({diff, 0});
            // If text is smaller than the textbox, force align on left
            if (textWidth < (getSize().x - padding * 2))
            {
                diff = padding - m_text.getPosition().x;
                m_text.move({diff, 0});
                m_cursor.move({diff, 0});
            }
        }
    }
}


size_t TextBox::getCursor() const
{
    return m_cursorPos;
}


void TextBox::onKeyPressed(const sf::Event::KeyEvent& key)
{
    switch (key.code)
    {
    case sf::Keyboard::Left:
        if (key.control)
        {
            // NOTE: assuming getString()[size] is a valid ref. (of the trailing \0)!

            setCursor(m_cursorPos - 1, key.shift);
            auto what_to_skip = m_text.getString()[m_cursorPos]; //! == ' ' will be checked to decide
            while (m_cursorPos > 0 &&
                   ((what_to_skip == ' ' && m_text.getString()[m_cursorPos - 1] == ' ')
                 || (what_to_skip != ' ' && m_text.getString()[m_cursorPos - 1] != ' '))) // sorry, the extra () noise is to shut GCC up (-Wparentheses)
                setCursor(m_cursorPos - 1, key.shift);
        }
        else if (key.shift)
        {
            if (m_cursorPos == m_selectionLast)
            {
                // Extend selection to left
                if (m_selectionFirst > 0)
                    setSelectedText(m_selectionFirst - 1, m_selectionLast);
            }
            else
            {
                // Shrink selection to right
                setSelectedText(m_selectionFirst, m_selectionLast - 1);
            }
        }
        else if (m_selectedText.isEmpty())
        {
            // Move cursor to the left
            setCursor(m_cursorPos - 1, key.shift);
        }
        else
        {
            // Clear selection, move cursor to the left
            setCursor(m_selectionFirst);
            clearSelectedText();
        }
        break;

    case sf::Keyboard::Right:
        if (key.control)
        {
            // NOTE: assuming getString()[size] is a valid ref. (of the trailing \0)!

            auto what_to_skip = m_text.getString()[m_cursorPos]; //! == ' ' will be checked to decide
            do {
                setCursor(m_cursorPos + 1, key.shift);
            } while (m_cursorPos < m_text.getString().getSize() &&
                     ((what_to_skip == ' ' && m_text.getString()[m_cursorPos] == ' ')
                   || (what_to_skip != ' ' && m_text.getString()[m_cursorPos] != ' '))); // sorry, the extra () noise is to shut GCC up (-Wparentheses)
        }
        else if (key.shift)
        {
            if (m_cursorPos == m_selectionFirst)
            {
                // Extend selection to right
                if (m_selectionLast < m_text.getString().getSize())
                    setSelectedText(m_selectionFirst, m_selectionLast + 1);
            }
            else
            {
                // Shrink selection to left
                setSelectedText(m_selectionFirst + 1, m_selectionLast);
            }
        }
        else if (m_selectedText.isEmpty())
        {
            // Move cursor to the right
            setCursor(m_cursorPos + 1, key.shift);
        }
        else
        {
            // Clear selection, move cursor to the right
            setCursor(m_selectionLast);
            clearSelectedText();
        }
        break;

    case sf::Keyboard::Backspace:
        if (!m_selectedText.isEmpty())
        {
            deleteSelectedText();
            break;
        }
        // Erase character before cursor
        if (m_cursorPos > 0)
        {
            sf::String string = m_text.getString();
            string.erase(m_cursorPos - 1);
            m_text.setString(string);

            setCursor(m_cursorPos - 1);
        }
        break;

    case sf::Keyboard::Delete:
        if (!m_selectedText.isEmpty())
        {
            deleteSelectedText();
            break;
        }
        // Erase character after cursor
        if (m_cursorPos < m_text.getString().getSize())
        {
            sf::String string = m_text.getString();
            string.erase(m_cursorPos);
            m_text.setString(string);

            setCursor(m_cursorPos);
        }
        break;

    case sf::Keyboard::Home:
        if (key.shift)
        {
            // Shift+Home: select from start to cursor
            setSelectedText(0, m_cursorPos);
        }
        else
        {
            setCursor(0);
        }
        break;

    case sf::Keyboard::End:
        if (key.shift)
        {
            // Shift+End: select from cursor to end
            setSelectedText(m_cursorPos, m_text.getString().getSize());
        }
        else
        {
            setCursor(m_text.getString().getSize());
        }
        break;

    case sf::Keyboard::Enter:
        triggerCallback();
        break;

    // Ctrl+A: select all
    case sf::Keyboard::A:
        if (key.control)
        {
            setSelectedText(0, m_text.getString().getSize());
        }
        break;

    // Ctrl+V: paste clipboard
    case sf::Keyboard::V:
        if (key.control)
        {
            // Delete selected text and write clipboard string over it.
            deleteSelectedText();
            sf::String string = m_text.getString();
            sf::String clipboardString = sf::Clipboard::getString();
            // Trim clipboard content if needed
            if ((string.getSize() + clipboardString.getSize()) > m_maxLength)
            {
                clipboardString = clipboardString.substring(0, m_maxLength - string.getSize());
            }
            // Insert string at cursor position
            string.insert(m_cursorPos, clipboardString);
            m_text.setString(string);
            setCursor(m_cursorPos + clipboardString.getSize());
        }
        break;

    // Ctrl+C: copy selected text to clipboard
    case sf::Keyboard::C:
        if (key.control)
        {
            if (!m_selectedText.isEmpty())
            {
                sf::Clipboard::setString(m_selectedText);
            }
        }
        break;

    // Ctrl+X: cut selected text to clipboard
    case sf::Keyboard::X:
        if (key.control)
        {
            if (!m_selectedText.isEmpty())
            {
                sf::Clipboard::setString(m_selectedText);
                deleteSelectedText();
            }
        }
        break;

    default:
        break;
    }
}


void TextBox::onMouseEnter()
{
    setMouseCursor(sf::Cursor::Text);
}


void TextBox::onMouseLeave()
{
    setMouseCursor(sf::Cursor::Arrow);
}


void TextBox::onMousePressed(float x, float)
{
    for (int i = (int)m_text.getString().getSize(); i >= 0; --i)
    {
        // Place cursor after the character under the mouse
        sf::Vector2f glyphPos = m_text.findCharacterPos(i);
        if (glyphPos.x <= x)
        {
            setCursor(i);
            break;
        }
    }
}


void TextBox::onMouseReleased(float x, float)
{
    for (int i = (int)m_text.getString().getSize(); i >= 0; --i)
    {
        // Place cursor after the character under the mouse
        sf::Vector2f glyphPos = m_text.findCharacterPos(i);
        if (glyphPos.x <= x)
        {
            setSelectedText(m_cursorPos, i);
            break;
        }
    }
}


void TextBox::onMouseMoved(float x, float)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        for (int i = (int)m_text.getString().getSize(); i >= 0; --i)
        {
            // Place cursor after the character under the mouse
            sf::Vector2f glyphPos = m_text.findCharacterPos(i);
            if (glyphPos.x <= x)
            {
                setSelectedText(m_cursorPos, i);
                break;
            }
        }
    }
}


void TextBox::onTextEntered(uint32_t unicode)
{
    if (unicode > 30 && (unicode < 127 || unicode > 159))
    {
        // Delete selected text when a new input is received
        deleteSelectedText();
        sf::String string = m_text.getString();
        if (string.getSize() < m_maxLength)
        {
            // Insert character in string at cursor position
            string.insert(m_cursorPos, unicode);
            m_text.setString(string);
            setCursor(m_cursorPos + 1);
        }
    }
}


void TextBox::onStateChanged(WidgetState state)
{
    m_box.applyState(state);

    // Discard selection when focus is lost
    if (state != WidgetState::Focused)
    {
        clearSelectedText();
    }
}


void TextBox::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_box, sfml_renderstates);

    // Crop the text with GL Scissor
    glEnable(GL_SCISSOR_TEST);

    sf::Vector2f pos = getAbsolutePosition();
    glScissor(
        (GLint)(pos.x + Theme::borderSize),
        (GLint)(ctx.target.getSize().y - (pos.y + getSize().y)),
        (GLsizei)getSize().x,
        (GLsizei)getSize().y
    );

    if (m_text.getString().isEmpty())
    {
        ctx.target.draw(m_placeholder, sfml_renderstates);
    }
    else
    {
        // Draw selection indicator
        if (!m_selectedText.isEmpty())
        {
            sf::RectangleShape selRect;
            const sf::Vector2f& startPos = m_text.findCharacterPos(m_selectionFirst);
            selRect.setPosition(startPos);
            selRect.setSize({m_text.findCharacterPos(m_selectionLast).x - startPos.x, m_cursor.getSize().y});
            selRect.setFillColor(Theme::input.textSelectionColor);
            ctx.target.draw(selRect, sfml_renderstates);
        }
        ctx.target.draw(m_text, sfml_renderstates);
    }

    glDisable(GL_SCISSOR_TEST);

    // Show cursor if focused
    if (isFocused())
    {
        // Make it blink
        float timer = m_cursorTimer.getElapsedTime().asSeconds();
        if (timer >= BLINK_PERIOD)
            m_cursorTimer.restart();

        // Hijacking draw() as a timer tick callback... (Hi five to Alexandre! ;) )
        sf::Color color = Theme::input.textColor;
        color.a = (m_cursorStyle == PULSE ? uint8_t(255 - (255 * timer / BLINK_PERIOD))
                                          : uint8_t(255 - (255 * timer / BLINK_PERIOD)) & 128 ? 255 : 0);
        m_cursor.setFillColor(color);

        ctx.target.draw(m_cursor, sfml_renderstates);
    }
}


void TextBox::setSelectedText(size_t from, size_t to)
{
    if (from != to)
    {
        size_t selectionLast = std::max(from, to);
        size_t selectionFirst = std::min(from, to);
        if (selectionFirst != m_selectionFirst || selectionLast != m_selectionLast)
        {
            m_selectionFirst = selectionFirst;
            m_selectionLast = selectionLast;
            m_selectedText = m_text.getString().substring(m_selectionFirst, m_selectionLast - m_selectionFirst);
        }
    }
    else
    {
        clearSelectedText();
    }
}


void TextBox::clearSelectedText()
{
    m_selectionFirst = m_selectionLast = m_cursorPos;
    m_selectedText.clear();
}


const sf::String& TextBox::getSelectedText() const
{
    return m_selectedText;
}


void TextBox::deleteSelectedText()
{
    // Delete if any selected text
    if (!m_selectedText.isEmpty())
    {
        sf::String str = m_text.getString();
        str.erase(m_selectionFirst, m_selectionLast - m_selectionFirst);
        setCursor(m_selectionFirst);
        clearSelectedText();
        m_text.setString(str);
    }
}


TextBox*  TextBox::setPlaceholder(const sf::String& placeholder)
{
    m_placeholder.setString(placeholder);

    return this;
}


const sf::String& TextBox::getPlaceholder() const
{
    return m_placeholder.getString();
}


TextBox* TextBox::setCallback(std::function<void(TextBox*)> callback)
{
    return (TextBox*) Widget::setCallback( [callback] (Widget* w) { callback( (TextBox*)w ); });
}

} // namespace
