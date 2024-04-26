#ifndef SFW_TOOLTIP_HPP
#define SFW_TOOLTIP_HPP

#include "sfw/Widget.hpp"

#include "sfw/Gfx/Elements/FilledRect.hpp"
#include "sfw/Gfx/Elements/Text.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>

#include <string>

namespace sfw {

class Tooltip : public Widget
{
// ---- Setup ----------------------------------------------------------------
	static constexpr float DELAY_TIME    = 0.75f; // s
	static constexpr float SUSTAIN_MIN   = 1.0f; // s
	static constexpr float READING_RATE  = 0.1f; // s/char
	static constexpr float FADEOUT_TIME  = 2.0f; // s

	enum State { Off, Delayed, Showing, Fadeout };

public:
	Tooltip(Widget* owner);
	Tooltip(Widget* owner, std::string text);
	Tooltip(const Tooltip&) = delete;
	Tooltip(Tooltip&&) = delete;

	void setText(const std::string& text);
	std::string getText() const { return m_text.get(); }

	void setState(State s);

// ---- Actions --------------------------------------------------------------
	void arm(float mouse_x = 0, float mouse_y = 0); //! Note: relative to the owner's *parent*!
		// If already visible, restart from the Showing ("sustain") phase,
		// cancelling a pending fadeout
		// (Default 0s to support drawing out-of-context, too. 
		//!!Might make a best-effort attempt then to still find
		//!!a good position. And if that works well enough, the
		//!!params might get dropped for the general case, too.)

	void show();
		// (Re)start from the Showing phase,
		// i.e. skip the Delayed, or cancel the Fadeout phase

	void dismiss();
		// Fadeout if already visible

// ---- Queries --------------------------------------------------------------
	bool visible() const;
	bool armed() const;
	bool elapsed(float time_interval);

// ---- Callbacks ------------------------------------------------------------
	void draw(const gfx::RenderContext& ctx) const override;
	void onTick() override;

// ---- Internal helpers -----------------------------------------------------
private:
	void initView();

// ---- Data -----------------------------------------------------------------
private:
	//!!Quick hack, until Widget becomes WidgetContainer...:
	//!!(See also the ctors, for hooking up with the "owner"!)
	Widget* m_owner; GUI* getMain() const { return m_owner ? m_owner->getMain() : nullptr; }

	State m_state{Off};
	FilledRect m_box;
	Text m_text;
	size_t m_length; // of the text (cached to spare the length queries)
	float m_timeStateChange;
};

} // namespace

#endif // SFW_TOOLTIP_HPP
