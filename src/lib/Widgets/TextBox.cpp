#include "sfw/Widgets/TextBox.hpp"
#include "sfw/Theme.hpp"
#include "sfw/GUI-main.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/OpenGL.hpp>

#include <cassert>
#include <algorithm>
    using std::min, std::max;
#include <functional>

#include <iostream> //!DEBUG
using namespace std;

#define BLINK_PERIOD 1.f

namespace sfw
{

//----------------------------------------------------------------------------
//!! These should take into account the writing direction,
//!! and flip the min/max selection accordingly!
size_t TextSelection::left()  const { return min(anchor_pos, active_pos); }
size_t TextSelection::right() const { return max(anchor_pos, active_pos); }

void TextSelection::start(size_t pos)
// Just blatantly overrides any pending selection proc.
{
	anchor_pos = active_pos = pos;
	following = true;
}

void TextSelection::set(size_t pos, size_t len)
// Disregard the follow mode! (Should be orhogonal to that.)
{
	anchor_pos = pos;
	resize(len);
}

void TextSelection::set_from_to(size_t anchor_pos_, size_t active_pos_)
// Disregard the follow mode! (Should be orhogonal to that.)
{
	anchor_pos = anchor_pos_;
	active_pos = active_pos_;
}

void TextSelection::stop()
{
        following = false;
}

void TextSelection::resume()
{
        following = true;
}

void TextSelection::reset()
{
        following = false;
	anchor_pos = 0;
	active_pos = 0;
}

void TextSelection::follow(size_t pos)
{
	if (following) active_pos = pos;
        else reset();
}

void TextSelection::resize(size_t len)
//!! NOTE: This may also change the anchor position! Not sure if that's always OK!
//!! At a quick glance it may just be mostly coincidence that it's in sync with the
//!! most common editing operations and it actually does what's expected... :)
//!!
//!! Anywhow, it violates the idea that Selection updates move the active pos and
//!! leave the anchor intact, unless *explicitly* requested otherwise!
{
	if (active_pos > anchor_pos) active_pos = anchor_pos + len;
	else                         anchor_pos = active_pos + len;
}


//----------------------------------------------------------------------------
TextBox::TextBox(float width, CursorStyle style):
    m_box(Box::Input),
    m_cursorStyle(style),
    m_cursorPos(0),
    m_maxLength(256)
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
    setCursorPos(0);

    setSize(m_box.getSize());
}


TextBox* TextBox::setText(const sf::String& string)
{
    m_text.setString(string.substring(0, m_maxLength)); // Limit the length
    setCursorPos(getText().getSize());

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
        setCursorPos(m_maxLength);
    }

    return this;
}


void TextBox::setCursorPos(size_t index)
{
//cerr << "setCursorPos-> cpos: " << index <<", anchor: " <<m_selection.anchor_pos << ", active: "<< m_selection.active_pos << ", follow: " << m_selection.following <<endl;

    if (index > m_text.getString().getSize()) // NOTE: a) The cursor pos. is unsigned.
                                              //       b) The pos. right after the end (technic'ly: at EOS) is OK.
    {
        // This "no-move" update kludge is to ensure the selection can update
        // its state on any navigation action, even when the cursor won't move!
        //!!Should instead call a generalized selection.onNavigation() here!
        m_selection.follow(getCursorPos());
        return;
    }

    m_cursorPos = index;
    m_selection.follow(index); // The selection itself will decide if and how exactly...

//cerr << "                     " << index <<", anchor: " <<m_selection.anchor_pos << ", active: "<< m_selection.active_pos << ", follow: " << m_selection.following <<endl;

    // The rest is all about adjusting the view (presentation/appearance)...

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


void TextBox::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::LShift || key.code == sf::Keyboard::RShift)
    {
        m_selection.stop();
    }
}


void TextBox::onKeyPressed(const sf::Event::KeyEvent& key)
{
    switch (key.code)
    {
    case sf::Keyboard::LShift:
    case sf::Keyboard::RShift:
        // If there's a selection, that means it's just been stopped.
        // Pressing Shift again in that case very likely means an intent to
        // resume it (probably for adjusting its size etc.), so allow that:
        if (m_selection) m_selection.resume();
        else m_selection.start(m_cursorPos);
        break;

    case sf::Keyboard::Left:
        if (key.control)
        {
            // NOTE: assuming getString()[size] is a valid ref. (of the trailing \0)!

            setCursorPos(m_cursorPos - 1);
            auto what_to_skip = m_text.getString()[m_cursorPos]; //! == ' ' will be checked to decide
            while (m_cursorPos > 0 &&
                   ((what_to_skip == ' ' && m_text.getString()[m_cursorPos - 1] == ' ')
                 || (what_to_skip != ' ' && m_text.getString()[m_cursorPos - 1] != ' '))) // sorry, the extra () noise is to shut GCC up (-Wparentheses)
                setCursorPos(m_cursorPos - 1);
        }
        else if (!key.shift && m_selection && getCursorPos() == m_selection.right())
        {
            // This is a special case for better ergonomics:
            // Resume the disappearing selection, and move the cursor to the left of it,
            // because what's actually desired is almost never to finish a selection,
            // do nothing with it, and then move the cursor a bit just to kill it...
            m_selection.resume();
            m_selection.set_from_to(getCursorPos(), m_selection.left());
            setCursorPos(m_selection.left());
            m_selection.stop();
        }
        else
        {
            // Just move the cursor normally
            setCursorPos(m_cursorPos - 1);
        }
        break;

    case sf::Keyboard::Right:
        if (key.control)
        {
            // NOTE: assuming getString()[size] is a valid ref. (of the trailing \0)!

            auto what_to_skip = m_text.getString()[m_cursorPos]; //! == ' ' will be checked to decide
            do {
                setCursorPos(m_cursorPos + 1);
            } while (m_cursorPos < m_text.getString().getSize() &&
                     ((what_to_skip == ' ' && m_text.getString()[m_cursorPos] == ' ')
                   || (what_to_skip != ' ' && m_text.getString()[m_cursorPos] != ' '))); // sorry, the extra () noise is to shut GCC up (-Wparentheses)
        }
        else if (!key.shift && m_selection && getCursorPos() == m_selection.left())
        {
            // This is a special case for better ergonomics:
            // Resume the disappearing selection, and move the cursor to the right of it,
            // because what's actually desired is almost never to finish a selection,
            // do nothing with it, and then move the cursor a bit just to kill it...
            m_selection.resume();
            m_selection.set_from_to(getCursorPos(), m_selection.right());
            setCursorPos(m_selection.right());
            m_selection.stop();
        }
        else
        {
            // Just move the cursor normally
            setCursorPos(m_cursorPos + 1);
        }
        break;

    case sf::Keyboard::Backspace:
        if (m_selection)
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

            setCursorPos(m_cursorPos - 1);
        }
        break;

    case sf::Keyboard::Delete:
        if (m_selection)
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

            setCursorPos(m_cursorPos);
        }
        break;

    case sf::Keyboard::Home:
        setCursorPos(0);
        break;

    case sf::Keyboard::End:
        setCursorPos(m_text.getString().getSize());
        break;

    case sf::Keyboard::Enter:
        triggerCallback();
        break;

    // Ctrl+A: select all
    case sf::Keyboard::A:
        if (key.control)
        {
            setSelection(0, m_text.getString().getSize());
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
            setCursorPos(m_cursorPos + clipboardString.getSize());
        }
        break;

    // Ctrl+C: copy selected text to clipboard
    case sf::Keyboard::C:
        if (key.control)
        {
            if (m_selection)
            {
                sf::Clipboard::setString(getSelectedText());
            }
        }
        break;

    // Ctrl+X: cut selected text to clipboard
    case sf::Keyboard::X:
        if (key.control)
        {
            if (m_selection)
            {
                sf::Clipboard::setString(getSelectedText());
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
    assert(getMain());
    getMain()->setMouseCursor(sf::Cursor::Text);
}


void TextBox::onMouseLeave()
{
    assert(getMain());
    getMain()->setMouseCursor(sf::Cursor::Arrow);
}


void TextBox::onMousePressed(float x, float)
{
    for (int i = (int)m_text.getString().getSize(); i >= 0; --i)
    {
        // Place cursor after the character under the mouse
        sf::Vector2f glyphPos = m_text.findCharacterPos(i);
        if (glyphPos.x <= x)
        {
//cerr << "onPeress-> i: " << i <<", anchor: " <<m_selection.anchor_pos << ", active: "<< m_selection.active_pos << ", follow: " << m_selection.following <<endl;

            setCursorPos(i);
//cerr << "              " << i <<", anchor: " <<m_selection.anchor_pos << ", active: "<< m_selection.active_pos << endl;

            // Must do this hackery after setCursorPos(), as follow() there would clean up an inactive selection!
            m_selection.reset(); //! This is a bit too eager, shouldn't start selecting just on a click,
                                  //! ...but must record the starting pos somehow, nonetheless! :-o
            m_selection.set_from_to(i,i);
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
            setCursorPos(i);
            break;
        }
    }
}


void TextBox::onMouseMoved(float x, float)
{
    if (getState() != WidgetState::Focused)
        return;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        for (int i = (int)m_text.getString().getSize(); i >= 0; --i)
        {
            // Place cursor after the character under the mouse
            sf::Vector2f glyphPos = m_text.findCharacterPos(i);
            if (glyphPos.x <= x)
            {
//cerr << "onMove-> anchor: " <<m_selection.anchor_pos << ", active: "<< m_selection.active_pos << endl;
                m_selection.resume();
                setCursorPos(i);
//cerr << "         anchor: " <<m_selection.anchor_pos << ", active: "<< m_selection.active_pos << endl;
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
            setCursorPos(m_cursorPos + 1);
        }
    }
}


void TextBox::onStateChanged(WidgetState state)
{
    m_box.applyState(state);

    // Discard selection when focus is lost
    if (state != WidgetState::Focused)
    {
        clearSelection();
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
        // Draw the selection indicator
        if (m_selection)
        {
            sf::RectangleShape selRect;
            const sf::Vector2f& startPos = m_text.findCharacterPos(m_selection.left());
            selRect.setPosition(startPos);
            selRect.setSize({m_text.findCharacterPos(m_selection.right()).x - startPos.x, m_cursor.getSize().y});
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

        // Hijacking draw() as a timer tick callback... :) Hi five to Alexandre@upstream for the brilliant idea!
        sf::Color color = Theme::input.textColor;
        color.a = (m_cursorStyle == PULSE ? uint8_t(255 - (255 * timer / BLINK_PERIOD))
                                          : uint8_t(255 - (255 * timer / BLINK_PERIOD)) & 128 ? 255 : 0);
        m_cursor.setFillColor(color);

        ctx.target.draw(m_cursor, sfml_renderstates);
    }
}


void TextBox::setSelection(size_t from, size_t to)
{
    m_selection.set(from, to - from);
}

void TextBox::clearSelection()
{
    m_selection.reset();
}


sf::String TextBox::getSelectedText() const
{
    return m_text.getString().substring(m_selection.left(), m_selection.length());
}


void TextBox::deleteSelectedText()
{
    // Delete the selected text, if any
    if (!m_selection.empty())
    {
        sf::String str = m_text.getString();
        str.erase(m_selection.left(), m_selection.length());
        setCursorPos(m_selection.left());
        m_text.setString(str);
    }
    clearSelection(); //! Must clear it even if empty, so it won't start growing "out of nothing"! (-> #159)
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
