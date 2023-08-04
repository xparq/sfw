#ifndef GUI_SLIDER_HPP
#define GUI_SLIDER_HPP

#include "sfw/InputWidget.hpp"
#include "sfw/Geometry.hpp"
#include "sfw/Gfx/Elements/Box.hpp"

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
			float size() const { return max - min; }
		};

		struct Cfg
		{
			Range range = {0, 100};
			float step = 1;
			Orientation orientation = Horizontal;

			// Direction by default: up/right > down/left
			bool invert = false;
			//!!bool invert_h = false;
			//!!bool invert_v = false;
			//!! Should ensure consistency among ^these^, while still allowing
			//!! simple prop-based configuration (without accessors)! Probably
			//!! via enums: .invert = Horizontal|Vertical
			// Use each arrow key, not just those matching the orientation
			bool use_all_arrow_keys = true;
			// Don't readjust the handle position on clicking it (#219)
			bool jumpy_thumb_click = false;
//!! NOT IMPL. YET:
//!!			// Continuously call the update callback on dragging
//!!			bool notify_on_drag = true;
		};
	}

class Slider: public InputWidget<Slider>
{
public:
	using Cfg = SliderConfig_GCC_CLANG_bug_workaround::Cfg;
	using Range = SliderConfig_GCC_CLANG_bug_workaround::Range;

public:
	Slider(const Cfg& cfg = Cfg()/*, const Style& style = Style()*/, float length = 200);

	// Value increment, or number of intervals
	Slider* setStep(float step); // step = 0 -> auto-adjust for continuous tracking
	float   step() const;
	Slider* setIntervals(float n);
	float   intervals() const;

	// Value range
	Slider*      setRange(float min, float max, bool continuous = true);
	Slider*      setRange(float min, float max, float step);
	const Range& range() const;
	float min() const;
	float max() const;

	// Current value
	Slider* set(float value);
	float   get() const;
	// Unfortunately, these are also needed to seamlessly support integers, too (without warnings):
	Slider* set(int value)      { return set((float)value); }
	Slider* set(unsigned value) { return set((float)value); }

	Slider* inc();
	Slider* dec();
	Slider* inc(float delta);
	Slider* dec(float delta);
	Slider* move(float delta);

private:
	// Helpers
	void updateGeometry(); // Sync the widget structure to GUI config/state
	void updateView(); // Sync the moving parts to the internal state
	float mousepos_to_sliderval(float x, float y) const;
	float sliderval_to_handledistance(float v) const;
	float track_length() const;

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
	Box m_thumb;
	sf::Vertex m_progression[4];
};


//----------------------------------------------------------------------------
inline const Slider::Range& Slider::range() const { return m_cfg.range; }
inline float Slider::step() const { return m_cfg.step; }
inline float Slider::intervals() const { return m_cfg.range.size() / step(); }
inline float Slider::min() const { return m_cfg.range.min; }
inline float Slider::max() const { return m_cfg.range.max; }
inline float Slider::get() const { return m_value; }
inline Slider* Slider::inc() { return move(step()); }
inline Slider* Slider::dec() { return move(-step()); }
inline Slider* Slider::inc(float delta) { return move(delta); }
inline Slider* Slider::dec(float delta) { return move(-delta); }

} // namespace

#endif // GUI_SLIDER_HPP
