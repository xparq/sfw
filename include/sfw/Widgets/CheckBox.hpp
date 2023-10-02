#ifndef _SFW_CHECKBOX_HPP_
#define _SFW_CHECKBOX_HPP_

#include "sfw/InputWidget.hpp"
#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/Gfx/Elements/CheckMark.hpp"

namespace sfw
{

/*****************************************************************************
  Standard vanilla checkbox with no surprises
  (Well, actually, one surprise could still be that it doesn't even have its
  own label... You'd need to add one yourself using a Form or HBox layout.)
 *****************************************************************************/
class CheckBox: public InputWidget<CheckBox>
{
public:
	CheckBox(bool checked = false);
	CheckBox(std::function<void(CheckBox*)> callback, bool checked = false);

	// "Generic-input-level" abstract get/set
	CheckBox* set(bool checked);
	bool get() const { return m_checked; }

	// Widget-specific operations
	CheckBox* check()   { return update(true); }
	CheckBox* uncheck() { return update(false); }
	CheckBox* toggle()  { return update(!checked()); }

	// Widget-specific queries
	operator bool() const { return checked(); }
	bool checked() const { return get(); }
	// Still keeping the legacy style, too:
	bool isChecked() const { return checked(); }

private:
	void draw(const gfx::RenderContext& ctx) const override;

	// Callbacks
	void onActivationChanged(ActivationState state) override;
	void onThemeChanged() override;
	void onMouseReleased(float x, float y) override;
	void onKeyPressed(const sf::Event::KeyEvent& key) override;

	// State
	Box m_box;
	CheckMark m_checkmark;
	bool m_checked;
};


} // namespace

#endif // _SFW_CHECKBOX_HPP_
