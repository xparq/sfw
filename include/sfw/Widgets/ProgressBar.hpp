#ifndef SFW_PROGRESS_BAR_HPP
#define SFW_PROGRESS_BAR_HPP

#include "sfw/Widget.hpp"
#include "sfw/Geometry.hpp"
#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/Gfx/Elements/Text.hpp"

namespace sfw
{

enum LabelPlacement
{
	LabelNone,   // Don't display the label
	LabelOver,   // Label over (inside) the progress bar
	LabelOutside // Label next to or below the bar
};

/*
   Basic horizontal/vertical progress bar

   Static output-only widget: can't be interacted with, or trigger events.
*/


	namespace _ProgressBar_GCC_CLANG_bug_workaround // https://stackoverflow.com/questions/53408962/try-to-understand-compiler-error-message-default-member-initializer-required-be
	{
		struct Range
		{
			float min = 0, max = 100;
			float size() const { return max - min; }
		};
		struct Cfg
		{
			float length = 200; // Widget width or height (in pixels), depending on orientation
			Range range = {0, 100};
	//		float step = 1;
			Orientation orientation = Horizontal;
	//		bool invert = false; // Default direction: down/left < up/right
			bool clamp = true; // Setting out-of-range values is disabled by default
			std::string unit = "!usedefault!"; //!! This `unit` field will be superceded by a versatile formatting pattern in teh future!
	//!!		std::string label; //!! Will be a formatting pattern, obsoleting `unit`!
			LabelPlacement label_placement = LabelOver;
		};
	}

class ProgressBar: public Widget
{
public:
	using Cfg   = _ProgressBar_GCC_CLANG_bug_workaround::Cfg;
	using Range = _ProgressBar_GCC_CLANG_bug_workaround::Range;

	ProgressBar(const Cfg& cfg = Cfg()/*, const Style& style = Style()*/);

	//!! DEPRECATED:
	/**
	 * @param length: Widget length in pixels (becomes width or height, according to orientation)
	 * @param orientation: Horizontal or Vertical
	 * @param labelPlacement: Where (or if) to show the label
	 */
	ProgressBar(float length = 200.f, Orientation orientation = Horizontal, LabelPlacement labelPlacement = LabelOver);


	ProgressBar* set(float value); // Percent (0 <= value <= 100) by default; see setRange()!
	float get() const;

	ProgressBar* setRange(float min, float max, std::string unit = "");
	const Range& range() const;
	float min() const;
	float max() const;

private:
	void draw(const gfx::RenderContext& ctx) const override;
	// Callbacks
	void onThemeChanged() override;
	// Helpers
	void updateGeometry();
	float track_length() const;
	float val_to_barlength(float v) const;

	enum VertexIndex : unsigned { TopLeft, BottomLeft, TopRight, BottomRight, _VERTEX_COUNT_ };

	Cfg m_cfg;

	float m_value;

	Box m_box;
	sf::Vertex m_bar[_VERTEX_COUNT_];
	Text m_label;
}; // ProgressBar


//----------------------------------------------------------------------------
inline const ProgressBar::Range& ProgressBar::range() const { return m_cfg.range; }
//inline float ProgressBar::step() const { return m_cfg.step; }
//inline float ProgressBar::intervals() const { return m_cfg.range.size() / step(); }
inline float ProgressBar::min() const { return m_cfg.range.min; }
inline float ProgressBar::max() const { return m_cfg.range.max; }

} // namespace sfw

#endif // SFW_PROGRESS_BAR_HPP
