#ifndef GUI_TEXTBOX_HPP
#define GUI_TEXTBOX_HPP

#include "sfw/InputWidget.hpp"
#include "sfw/Gfx/Elements/Text.hpp"
#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/TextSelection.hpp"

#include <string>

#include <SFML/System/String.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Clock.hpp>

namespace sfw
{

/*****************************************************************************
  The TextBox widget is a one-line plain-text editor with selection support.
  All the strings expected or returned by the operations are UTF-8 encoded.
 *****************************************************************************/
class TextBox: public InputWidget<TextBox>
{
public:
	enum CursorStyle
	{
		BLINK,
		PULSE
	};

	constexpr static size_t   DefaultMaxLength = 256;
	constexpr static uint32_t DefaultBoxWidth = 200;

	TextBox(float pxWidth = DefaultBoxWidth, CursorStyle style = BLINK);

	// Set/get content
	TextBox* set(const std::string& content);
	std::string get() const;
	std::string getSelected() const;

	// Content length (slightly incorrectly) is the "number of Unicode code-points"
	size_t length() const;

	// Set content length limit
	TextBox* setMaxLength(size_t maxLength);

	// Set/get cursor position (char index, starting with 0; the position
	// after the last char (i.e. at length()) is also valid)
	void   setCursorPos(size_t pos);
	size_t getCursorPos() const { return m_cursorPos; }

	TextBox*    setPlaceholder(const std::string& placeholder);
	std::string getPlaceholder() const;

	//------------------------------------------------------------------------
	// Legacy support for SFML strings
	// (Will be done in an automatically backand-matched derived variant class in the future!)
	TextBox*   setString(const sf::String& content);
	sf::String getString() const;
	sf::String getSelectedString() const;
	TextBox*   setPlaceholderString(const sf::String& placeholder);
	sf::String getPlaceholderString() const;

	//------------------------------------------------------------------------
	// Commands (for binding to input events; note the PascalCase for these)
	void PrevPos();
	void NextPos();
	void SkipBackward(); // to prev. "word" boundary
	void SkipForward(); // to next "word" boundary
	void Home();
	void End();
	void DelPrevChar(); // "Backspace"
	void DelNextChar(); // "Delete"
	void SelectAll();
	void Copy();
	void Cut();
	void Paste();
	// "Macros" (compound actions built from the ones above)
	void Backward(bool skip_to_boundary = false);
	void Forward(bool skip_to_boundary = false);
	void DelBackward(); // to prev. "word" boundary
	void DelForward(); // to next "word" boundary

protected:
	// Internal helpers
	void set_selection(size_t from, size_t to);
	void clear_selection();
	void delete_selected();
	bool flip_selection(const sf::Event::KeyEvent& key, size_t from, size_t to);
	void update_view();
	size_t pos_at_mouse(float mouse_x);

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
	void onMouseWheelMoved(int delta) override;
	void onTextEntered(char32_t unichar) override;
	void onStateChanged(WidgetState state) override;
	void onThemeChanged() override;

	// Config:
	size_t        m_maxLength;
	float         m_pxWidth;
	CursorStyle   m_cursorStyle;
	float         m_cursorWidth = 1; // pixel
	float         m_cursorBlinkPeriod = 1; // s
	Text          m_placeholder;
	// Internal editor state:
	Text          m_text; //!! <- well, this also contains the visual state of the text!
	size_t        m_cursorPos; // (Not a property of the visual cursor representation!)
	TextSelection m_selection;
	// Widget visual state:
	Box m_box;
	mutable sf::RectangleShape m_selectionMarker;
	// Cursor visual state:
	/*!!
	struct CursorView
	{
		mutable sf::Color          color; // for blinking, not the configured cursor color!
		mutable sf::RectangleShape rect;
		mutable sf::Clock          timer;
	};
	mutable CursorView m_cursor;
	!!*/
	mutable sf::RectangleShape m_cursorRect;
	mutable sf::Color m_cursorColor;
	mutable sf::Clock m_cursorTimer;
};

} // namespace

#endif // GUI_TEXTBOX_HPP
