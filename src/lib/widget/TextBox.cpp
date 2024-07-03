#include "sfw/widget/TextBox.hpp"

#include "sfw/Theme.hpp"
#include "sfw/GUI-main.hpp"
#include "sfw/util/utf8.hpp"
#include "sfw/adapter/sfml.hpp"
#include "sfw/util/diagnostics.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/OpenGL.hpp>

#include <cassert>
#include <algorithm>
	using std::min, std::max;


#define CFG_KEEP_SELECTION_ON_SHIFT_LEFT_RIGHT
#define CFG_KEEP_SELECTION_ON_NEW_SHIFT


namespace sfw
{

//----------------------------------------------------------------------------
// TextBox
//----------------------------------------------------------------------------
//
// NOTES:
//
// - update_view() syncs most of the visuals (like the position of the text
//   and the cursor, or the selection highlight) to the internal editor state,
//   so it needs to be called after (almost) every change (or set of changes).
//   (Some of the "view" aspects, like actually rendering the updated text,
//   or blinking the cursor, are done by draw() directly, in every frame!)
//   Since currently it's called by setCursorPos(), actions involving cursor
//   movement are implicitly taken care of. (If moving the cursor is not the
//   last change they make, it needs more careful consideration, though.)
//   Others (i.e. those not moving the cursor, like Delete, or those deleting
//   the selected text etc.) need to call it explicitly.
//

TextBox::TextBox(float pxWidth, CursorStyle style):
	m_maxLength(DefaultMaxLength),
	m_pxWidth(pxWidth),
	m_box(Box::Input)
{
	// Visuals
	m_cursorStyle = style;
	onThemeChanged(); //!!Kludge to force geom. recalc.!

	// Mechanics - need to be done after the visuals, as the visual part of
	// the cursor positioning depends on the visual setup!
	Home();
}


TextBox* TextBox::set(const std::string& content)
{
	m_text.set(sfw::utf8_substr(content, 0, m_maxLength)); // Limit the length
	setCursorPos(length()); // End(), but it's unclear if it'd be too high-level here...

	setChanged(); //!! Will become more sophisticated with Undo/Redo etc.
	return this;
}

std::string TextBox::get() const
{
	return m_text.get();
}

std::string TextBox::getSelected() const
{
	return SFMLString_to_stdstring(
		m_selection.empty() ? "" : m_text.getString().substring(m_selection.lower(), m_selection.length())
	);
	//!!Decoupled from SFML, but slower (O(N)):
	//!!	return utf8_substr(m_text.get(), m_selection.lower(), m_selection.length());
}


size_t TextBox::length() const
{
	//!!NOT UTF8-aware: return get().length();
	return m_text.getString().getSize();
}


TextBox* TextBox::setMaxLength(size_t maxLength)
{
	//!!Don't do it this way, creating an "invalid" period (another nail in the coffin of thread-safety)!
	m_maxLength = maxLength;
	// Trim current text if needed
	if (length() > m_maxLength)
	{
		set(get());
	}

	return this;
}


TextBox*  TextBox::setPlaceholder(const std::string& placeholder)
{
	m_placeholder.set(placeholder);

	return this;
}

std::string TextBox::getPlaceholder() const
{
	return m_placeholder.get();
}


void TextBox::setCursorPos(size_t pos)
{
	if (pos > length()) // NOTE: a) The cursor pos. is unsigned.
	                    //       b) The pos. right after the end (at EOS) is OK.
	{
		// Since a previously stopped (volatile) selection would need to be cancelled on
		// the next action -- even if it won't do anything (like move the cursor) --,
		// here we need to make sure a "null" navigation event also reaches the selection:
		m_selection.follow(getCursorPos());
		return;
	}

	m_cursorPos = pos;
	m_selection.follow(pos); // The selection itself will decide if and how exactly...

	update_view();
}


//--- Commands ---------------------------------------------------------------

void TextBox::Home()
{
	setCursorPos(0);
}

void TextBox::End()
{
	setCursorPos(length());
}

void TextBox::PrevPos()
{
	setCursorPos(m_cursorPos - 1);
}

void TextBox::NextPos()
{
	setCursorPos(m_cursorPos + 1);
}

void TextBox::SkipBackward()
{
	setCursorPos(m_cursorPos - 1);
	// NOTE: assuming getString()[size] is a valid ref. of the trailing '\0' (as with C++11 std::string)!
	auto what_to_skip = m_text.getString()[m_cursorPos]; //! == ' ' will be checked to decide
	while (m_cursorPos > 0 &&
	       (   (what_to_skip == ' ' && m_text.getString()[m_cursorPos - 1] == ' ')
	        || (what_to_skip != ' ' && m_text.getString()[m_cursorPos - 1] != ' '))) // the extra () is to shut GCC up (-Wparentheses) :-/
	setCursorPos(m_cursorPos - 1);
}

void TextBox::SkipForward()
{
	// NOTE: assuming getString()[size] is a valid ref. of the trailing '\0' (as with C++11 std::string)!
	auto what_to_skip = m_text.getString()[m_cursorPos]; //! == ' ' will be checked to decide
	do {
		setCursorPos(m_cursorPos + 1);
	} while (m_cursorPos < length() &&
	         (   (what_to_skip == ' ' && m_text.getString()[m_cursorPos] == ' ')
	          || (what_to_skip != ' ' && m_text.getString()[m_cursorPos] != ' '))); // the extra () is to shut GCC up (-Wparentheses) :-/
}

void TextBox::Backward(bool skip)
{
	if (skip) SkipBackward(); else PrevPos();
}

void TextBox::Forward(bool skip)
{
	if (skip) SkipForward(); else NextPos();
}

void TextBox::DelPrevChar() // "Backspace"
{
	if (m_cursorPos > 0)
	{
		sf::String string = m_text.getString();
		string.erase(m_cursorPos - 1);
		m_text.setString(string);

		setCursorPos(m_cursorPos - 1);
		//update_view(); // setCursorPos has just called it
	}
}

void TextBox::DelNextChar() // "Delete"
{
	if (m_cursorPos < length())
	{
		sf::String string = m_text.getString();
		string.erase(m_cursorPos);
		m_text.setString(string);
		update_view();
	}
}

void TextBox::DelBackward() // to prev. "word" boundary
{
	m_selection.start(getCursorPos());
	SkipBackward(); // The selection will follow!
	delete_selected();
}

void TextBox::DelForward() // to next "word" boundary
{
	m_selection.start(getCursorPos());
	SkipForward(); // The selection will follow!
	delete_selected();
}

void TextBox::SelectAll()
{
	// Don't stay at a "random" pos, to allow a next Shift+navig. action to work
	End(); // Also: call it first, otherwise it would cancel the selection!

	set_selection(0, length());
	update_view();
}

void TextBox::Copy()
{
	if (m_selection)
	{
//		sf::Clipboard::setString(stdstring_to_SFMLString(getSelected()));
		sf::Clipboard::setString(getSelectedString());
	}
}

void TextBox::Cut()
{
	if (m_selection)
	{
//		sf::Clipboard::setString(stdstring_to_SFMLString(getSelected()));
		sf::Clipboard::setString(getSelectedString());
		delete_selected();
	}
}

void TextBox::Paste()
{
	std::string clip = SFMLString_to_stdstring(sf::Clipboard::getString());
	auto cliplen = utf8_cpsize(clip);
	// Refuse to change if it would overflow
	if (length() + cliplen - m_selection.length() > m_maxLength)
	{
		//!!Visual error feedback (like flashing a red frame + tooltip)!
		return;
	}

	// If there's a selection, get it replaced:
	delete_selected();
	// Insert clipboard content at the cursor
	std::string newcontent = get();
	auto u8bpos = utf8_bsize(newcontent, m_cursorPos);
	newcontent.insert(u8bpos, clip);
	m_text.set(newcontent); //! not this->set(), to preserve the cursor pos.
	// Go to the end of the inserted part (or EOS)
	setCursorPos(m_cursorPos + cliplen);
}


//----------------------------------------------------------------------------
//--- Internal helpers -------------------------------------------------------
//----------------------------------------------------------------------------

void TextBox::set_selection(size_t from, size_t length)
{
	m_selection.start(from);
	m_selection.follow(from + length);
	m_selection.stop(); // <- May not be desired in all cases! (Add a flag to this fn. as needed then.)
}

void TextBox::clear_selection()
{
	m_selection.cancel();  //!!?? m_selection.reset();
}

void TextBox::delete_selected()
{
	// Delete the selected text, if any
	if (!m_selection.empty())
	{
		sf::String str = m_text.getString();
		str.erase(m_selection.lower(), m_selection.length());
		setCursorPos(m_selection.lower());
		m_text.setString(str);
	}
	clear_selection(); //! Must clear it even if empty, so it won't start growing "out of nothing"! (-> #159)

	update_view();
}

bool TextBox::flip_selection([[maybe_unused]] const sf::Event::KeyChanged& key,
                            [[maybe_unused]] size_t from, [[maybe_unused]] size_t to)
{
#ifdef CFG_KEEP_SELECTION_ON_SHIFT_LEFT_RIGHT
	if (!SFML_keypress_has_modifiers(key) && m_selection && getCursorPos() == from)
	{
		m_selection.resume(); // It's already stopped (by releasing Shift), so setCursorPos would kill it without this!
		m_selection.set_from_to(getCursorPos(), to);
		setCursorPos(to);
		m_selection.stop();
		return true;
	}
#endif
	return false;
}


size_t TextBox::pos_at_mouse(float mouse_x)
{
	size_t pos;
	for (pos = length(); pos > 0; --pos)
	{
		sf::Vector2f glyphPos = m_text.findCharacterPos(pos);
		if (glyphPos.x <= mouse_x)
			break;
	}
	return pos;
}


//----------------------------------------------------------------------------
void TextBox::update_view()
// Adjust the visuals after logical state changes...
{
	float framing_offset = Theme::borderSize + Theme::PADDING;
	float inrect_xmin = framing_offset;
	float inrect_xmax = getSize().x() - framing_offset;
	float inrect_y = framing_offset;

	m_cursorRect.setPosition({m_text.findCharacterPos(m_cursorPos).x, inrect_y});

	// Make sure the cursor is in view...
	float diff = 0;
	if (float curpos_px = m_cursorRect.getPosition().x;
	          curpos_px > inrect_xmax)  {   // Cur. pos. is off-rect to the right?
		diff = inrect_xmax - curpos_px; //   <- Shift left
	} else if (curpos_px < inrect_xmin) {   // Cur. pos. is off-rect to the left?
		diff = inrect_xmin - curpos_px; //   -> Shift right
	}
	m_text.move({diff, 0});
	m_cursorRect.move({diff, 0});

	auto text_x = m_text.position().x();
	//auto bounds = m_text.getLocalBounds(); float sfml_text_offset = bounds.left;
	// Sigh. These are all sometimes the same, sometimes slightly different... :-/
	//float textWidth_raw = bounds.width;
	//float textWidth_adj = textWidth_raw - sfml_text_offset;
	float textWidth_pos = m_text.findCharacterPos(length()).x - text_x;
	// I had the best results with this:
	auto textWidth = textWidth_pos;
/*
cerr << ": textWidth_raw, textWidth_adj, textWidth_pos: " << textWidth_raw  << ", " << textWidth_adj << ", " << textWidth_pos << endl;
cerr << ": Theme::borderSize, Theme::PADDING: " << Theme::borderSize << ", "<< Theme::PADDING
	 << ", inrect_xmin: " << inrect_xmin << ", " << "inrect_xmax: " << inrect_xmax << ", " << "m_cursorWidth: " << m_cursorWidth
	 << ", m_text.getPosition().x: " << m_text.getPosition().x
	 << endl;
*/

	// If the text overflows to the left, but there's still space on the right, align right...
	if (text_x < inrect_xmin && text_x + textWidth < inrect_xmax - m_cursorWidth)
	{
/*
cerr << getName() << " [" << get() << "] -> text.localbounds.left = "  << m_text.getLocalBounds().left << ", "
	 << ".width = " << m_text.getLocalBounds().width;
cerr << " -------- getSize().x = " << getSize().x << endl;
cerr << "- diff1 (->) = "<< diff;
cerr << endl;
*/
		// ...but if the text is shorter than the box, align left!
		if (textWidth < inrect_xmax - inrect_xmin) {
			diff = inrect_xmin - m_text.position().x();
		} else {
			diff = inrect_xmax - m_cursorWidth - (text_x + textWidth);
		}
		m_text.move({diff, 0});
		m_cursorRect.move({diff, 0});
	}

	//!!Moved these here from the end of onThemeChanged (and put a call to update_view() there!), to fix #300:
	//!!But I really don't understand, why what exactly caused the problem (a regression, I suppose!), and
	//!!how exactly this fixes it... :-o
	//!!
	//!!This should also adjust the x offset (later...)! (See notes at onThemeChanged!)
	m_text.reposition({m_text.position().x(), framing_offset});
	m_cursorRect.setPosition({m_cursorRect.getPosition().x, framing_offset}); //!! Sigh, mixing the two APIs...

	// Also update the selection highlight...
	if (m_selection)
	{
		const sf::Vector2f& start = m_text.findCharacterPos(m_selection.lower());
		m_selectionMarker.setPosition(start);
		m_selectionMarker.setSize({m_text.findCharacterPos(m_selection.upper()).x - start.x,
		                           m_cursorRect.getSize().y});
		m_selectionMarker.setFillColor(Theme::input.textSelectionColor);
	}

	// Reset the cursor blink period...
	m_cursorTimer.restart();
}


//----------------------------------------------------------------------------
//--- Event handlers ---------------------------------------------------------
//----------------------------------------------------------------------------

void TextBox::onKeyReleased(const sf::Event::KeyChanged& key)
{
	if (key.code == sf::Keyboard::Key::LShift || key.code == sf::Keyboard::Key::RShift)
	{
		m_selection.stop();
	}
}


void TextBox::onKeyPressed(const sf::Event::KeyChanged& key)
{
	switch (key.code)
	{
	case sf::Keyboard::Key::LShift:
	case sf::Keyboard::Key::RShift:
		// If there's a (volatile) selection, here that means it's just been stopped.
		// Pressing Shift again then could be interpreted as an intent to resume it
		// (for adjusting its extent), so it's reasonable to allow that.
		//
		// But... It could as well be a mistaken selection that the user is just
		// trying to start right over -- especially as <- and -> also may not cancel
		// the selection (but move to opposite ends of it) -- so, perhaps don't?...
		// (Note: <- and -> can still kill a volat. sel. if moving "outside" of it;
		// it's just more cognitive cost than "any Shift-less move kills it".)
		//
		// (BTW: re-entering the widget with Shift+Tab (or pressing any other key
		// combinations with Shift) definitely shouldn't reactivate a selection by
		// accident! ;) That doesn't apply here, as this is a standalone, bare Shift
		// keypress event, but let's sitck this reminder here nonetheless...)
#ifdef CFG_KEEP_SELECTION_ON_NEW_SHIFT
		if (m_selection)
			// Keep the (just finished) selection
			m_selection.resume();
		else
#endif
			// Start new selection
			m_selection.start(m_cursorPos);

		break;

	//------------------------------------------------------------------------
	// For Left/Right, there's a special case for better ergonomics, in case
	// a) there's a selection, and b) the cursor is at its end, and c) the move
	// points "inward":
	// Instead of letting the selection disappear, move the cursor to the opposite
	// end of the selection (and internally invert it, but that's not important).
	// This is more likely to be useful than to 1. finish a selection, then
	// 2. do nothing with it, and then 3. move the cursor a bit and kill it...
	// (See flip_selection() for more!)
	//------------------------------------------------------------------------
	case sf::Keyboard::Key::Left:
		if (!flip_selection(key, m_selection.upper(), m_selection.lower()))
			Backward(key.control);
		break;

	case sf::Keyboard::Key::Right:
		if (!flip_selection(key, m_selection.lower(), m_selection.upper()))
			Forward(key.control);
		break;

	case sf::Keyboard::Key::Backspace:
		if (m_selection) delete_selected(); //!!??Is there a hidden UC with _has_modifiers(key) && m_selection?
		else if (key.control) DelBackward();
		else DelPrevChar();
		break;

	case sf::Keyboard::Key::Delete:
		if (key.shift) Cut();
		else if (m_selection) delete_selected(); //!!??Is there a hidden UC with _has_modifiers(key) && m_selection?
		else if (key.control) DelForward();
		else DelNextChar();
		break;

	case sf::Keyboard::Key::Home:
	case sf::Keyboard::Key::Up:
		Home();
		break;

	case sf::Keyboard::Key::End:
	case sf::Keyboard::Key::Down:
		End();
		break;

	// "Apply"
	case sf::Keyboard::Key::Enter:
		setChanged(); //!! None of the inidividual editing actions update this yet,
		              //!! so we just force it here for the time being...
		updated(); //!!TBD: Should it be forced even if !changed()?
		break;

	// Ctrl+A: Select All
	case sf::Keyboard::Key::A:
		if (key.control) SelectAll();
		break;

	// Ctrl+V: Paste
	case sf::Keyboard::Key::V:
		if (key.control) Paste();
		break;

	// Ctrl+C: Copy
	case sf::Keyboard::Key::C:
		if (key.control) Copy();
		break;

	// Ctrl+X: Cut
	case sf::Keyboard::Key::X:
		if (key.control) Cut();
		break;

	// Ctrl+Insert: Copy, Shift+Insert: Paste
	case sf::Keyboard::Key::Insert:
		if (key.control) Copy();
		else if (key.shift) Paste();
		break;

	default: // To shut up GCC about "warning: enumeration value ... not handled"
		break;
	}
}


void TextBox::onMouseEnter()
{
	assert(getMain());
	getMain()->setMouseCursor(sf::Cursor::Type::Text);
}

void TextBox::onMouseLeave()
{
	assert(getMain());
	getMain()->setMouseCursor(sf::Cursor::Type::Arrow);
}


void TextBox::onMousePressed(float x, float)
{
	size_t pos = pos_at_mouse(x);
	setCursorPos(pos);
	m_selection.start(pos); // This looks a bit too eager here: shouldn't
	                        // start selecting just by a click, but
	                        // a) must record the start pos in case it's
	                        //    indeed gonna be a selection, and
	                        // b) mouse-release will stop it later anyway
	                        //    (as empty if not moved), so no harm done...
}


void TextBox::onMouseReleased(float, float)
{
	m_selection.stop();
}


void TextBox::onMouseMoved(float x, float)
{
	if (getActivationState() != ActivationState::Focused)
		return;

	// Go to char at mouse, starting/extending selection
	// (which is handled implicitly by setCursorPos)
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		size_t pos;
		if (x < Theme::borderSize + Theme::PADDING)
			pos = 0;
		else
			pos = pos_at_mouse(x);
		setCursorPos(pos);
	}
}


void TextBox::onMouseWheelMoved(int delta)
{
	auto ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
	            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);

	if (delta < 0) Forward(ctrl);
	else           Backward(ctrl);
}


void TextBox::onTextEntered(char32_t unichar)
{
	// Ignore some control code ranges
	if (unichar >= 32 && (unichar < 127 || unichar >= 160))
	{
		delete_selected(); // Delete selected text on entering a new char.

		if (sf::String content = m_text.getString(); content.getSize() < m_maxLength)
		{
			// Insert character at the cursor
			content.insert(m_cursorPos, unichar);
			m_text.setString(content);
			setCursorPos(m_cursorPos + 1);
		}
	}
}


void TextBox::onActivationChanged(ActivationState state)
{
	m_box.applyState(state);

	// Discard selection when focus is lost
	if (state != ActivationState::Focused)
	{
		clear_selection();
	}
}


void TextBox::onThemeChanged()
{
	float framing_offset = Theme::borderSize + Theme::PADDING;

//!! The repositionings below are incomplete alone! x needs readjusting, too!
//!! Ideally no repos. should even be needed, but a clean, net inner rect
//!! should be used instead for all the text + cursor drawing, but... later.
//!!
//!! Alternatively, somewhere before draw() (or latest: right there -- but
//!! of course that would be stupidly wasteful), there should be an internal
//!! update_view() call to iron out any possible inconsistencies (irrespective
//!! of what may have caused them)! And it would be called from onResize, too,
//!! if that becomes a thing (likely for a multi-line TextBox in the future).

	m_text.setFont(Theme::getFont());
	m_text.setFillColor(Theme::input.textColor);
	m_text.setCharacterSize((unsigned)Theme::textSize);

	m_placeholder.setFont(Theme::getFont());
	m_placeholder.setFillColor(Theme::input.textPlaceholderColor);
	m_placeholder.setCharacterSize((unsigned)Theme::textSize);
	//!! This is a "static fixture", can't move, so it's *probably* OK to
	//!! reposition it only once per theme change:
	m_placeholder.setPosition({framing_offset, framing_offset});

	m_cursorColor = Theme::input.textColor;
	m_cursorRect.setFillColor(Theme::input.textColor);
	//!! Insert/Overwrite would change it too, so this is not future-proof here at all:
	m_cursorRect.setSize(sf::Vector2f(m_cursorWidth, //!! Theme::textCursorWidth
	(float)Theme::getLineSpacing()));

	m_box.setSize(m_pxWidth, Theme::getBoxHeight());
	setSize(m_box.getSize());

	update_view();
}


//----------------------------------------------------------------------------
void TextBox::draw(const gfx::RenderContext& ctx) const
{
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	m_box.draw({ctx.target, sfml_renderstates});

	// Crop the text with GL Scissor
	glEnable(GL_SCISSOR_TEST);

	fVec2 pos = getAbsolutePosition();
	auto width = max(0.f, getSize().x() - 2 * Theme::borderSize - 2 * Theme::PADDING); // glScissor will fail if < 0!

	glScissor(
		(GLint)(pos.x() + Theme::borderSize + Theme::PADDING),
		(GLint)(ctx.target.getSize().y - (pos.y() + getSize().y())), //!! Sigh... Horrid mix of two APIs, again!... :-/
		(GLsizei)width,
		(GLsizei)getSize().y()
	);
	//!!Original: (tends to overflow the input rect -- how come it worked upstream?! :-o )
	//!!glScissor(pos.x + Theme::borderSize, ctx.target.getSize().y - (pos.y + getSize().y), getSize().x, getSize().y);

	if (m_text.getString().isEmpty())
	{
		m_placeholder.draw({ctx.target, sfml_renderstates});
	}
	else
	{
		// Draw the selection highlight as a background rect.
		if (m_selection)
			ctx.target.draw(m_selectionMarker, sfml_renderstates);
		// Draw the text
		m_text.draw({ctx.target, sfml_renderstates});
	}

	glDisable(GL_SCISSOR_TEST);
/*
	sf::RectangleShape clip;
	clip.setPosition({Theme::borderSize + Theme::PADDING, 0});
	clip.setSize({width, getSize().y});
	clip.setOutlineThickness(1);
	clip.setFillColor(sf::Color(0));
	clip.setOutlineColor(sf::Color::Green);
	ctx.target.draw(clip, sfml_renderstates);
*/
	// Show cursor if focused
	if (focused())
	{
		// Make it blink
		// Hijacking draw() as a timer tick callback... :) Hi five to Alexandre@upstream for the brilliant idea!
		float timer = m_cursorTimer.getElapsedTime().asSeconds();
		if (timer >= m_cursorBlinkPeriod) {
			m_cursorTimer.restart();
		}

		m_cursorColor.a = (m_cursorStyle == PULSE ? uint8_t(255 - (255 * timer / m_cursorBlinkPeriod))
		                                          : uint8_t(255 - (255 * timer / m_cursorBlinkPeriod)) & 128 ? 255 : 0);
		m_cursorRect.setFillColor(m_cursorColor);
		ctx.target.draw(m_cursorRect, sfml_renderstates);
	}

	//!!??Not needed now, but could be here: glDisable(GL_SCISSOR_TEST);
}


//------------------------------------------------------------------------
// Direct support for SFML strings
//!!Will be done in an automatically backand-matched derived variant class later!
TextBox* TextBox::setString(const sf::String& content)
{
	return set(SFMLString_to_stdstring(content));
}

sf::String TextBox::getString() const
{
	return m_text.getString();
}

sf::String TextBox::getSelectedString() const
{
	return m_selection.empty() ? "" : m_text.getString().substring(m_selection.lower(), m_selection.length());
}

TextBox*  TextBox::setPlaceholderString(const sf::String& placeholder)
{
	m_placeholder.setString(placeholder);
	return this;
}

sf::String TextBox::getPlaceholderString() const
{
	return m_placeholder.getString();
}

} // namespace
