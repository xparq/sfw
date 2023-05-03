#ifndef GUI_SLIDER_HPP
#define GUI_SLIDER_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/Geometry.hpp"

namespace sfw
{

/****************************************************************************
   Slider widget with vertical or horizontal appearance
 ****************************************************************************/

	namespace SliderConfig_GCC_CLANG_bug_workaround // https://stackoverflow.com/questions/53408962/try-to-understand-compiler-error-message-default-member-initializer-required-be
	{
		struct Range
		{
			float min, max;
		};

		struct Cfg
		{
			Range range = {0, 100};
			float step = 10;
			Orientation orientation = Horizontal;

			// Preferred increase/decrase direction
			bool invert = false;
			// Use each arrow key, not just those matching the orientation
			bool use_all_arrow_keys = true;
			// Don't readjust the handle position on clicking it (#219)
			bool jumpy_thumb_click = false;
		};
	}

class Slider: public Widget
{
	using Cfg = SliderConfig_GCC_CLANG_bug_workaround::Cfg;
	using Range = SliderConfig_GCC_CLANG_bug_workaround::Range;

public:
	Slider(const Cfg& cfg = Cfg()/*, const Style& style = Style()*/, float length = 200);

	// Position increment
	Slider* setStep(float step);
	float step() const;

	// Position increment
	Slider* setRange(float min, float max);
	const Range& range() const;
	float min() const;
	float max() const;

	// Current value
	Slider* set(float value);
	float get() const;

	Slider* inc();
	Slider* dec();
	Slider* inc(float delta);
	Slider* dec(float delta);
	Slider* move(float delta);

	// Set the onUpdate callback
	Slider* setCallback(std::function<void(Slider*)> callback);
	Slider* setCallback(std::function<void()> callback)         { return (Slider*) Widget::setCallback(callback); }

private:
	// Helpers
	void updateGeometry(); // Sync the widget structure to GUI config/state
	void updateView(); // Sync the moving parts to the internal state
	float mousepos_to_sliderval(float x, float y) const;
	float sliderval_to_handledistance(float v) const;

	void draw(const gfx::RenderContext& ctx) const override;

	// Callbacks
	void onKeyPressed(const sf::Event::KeyEvent& key) override;
	void onMousePressed(float x, float y) override;
	void onMouseMoved(float x, float y) override;
	void onMouseReleased(float x, float y) override;
	void onMouseWheelMoved(int delta) override;
	void onStateChanged(WidgetState state) override;
	void onThemeChanged() override;

	// Config:
	Cfg m_cfg;
	float m_boxLength;
	// Internal ("model") state:
	float m_value;
	// Visual ("view") state:
	Box m_track;
	Box m_handle;
	sf::Vertex m_progression[4];
};


//----------------------------------------------------------------------------
inline const Slider::Range& Slider::range() const { return m_cfg.range; }
inline float Slider::step() const { return m_cfg.step; }
inline float Slider::min() const { return m_cfg.range.min; }
inline float Slider::max() const { return m_cfg.range.max; }
inline float Slider::get() const { return m_value; }
inline Slider* Slider::inc() { return move(step()); }
inline Slider* Slider::dec() { return move(-step()); }
inline Slider* Slider::inc(float delta) { return move(delta); }
inline Slider* Slider::dec(float delta) { return move(-delta); }

} // namespace

#endif // GUI_SLIDER_HPP
