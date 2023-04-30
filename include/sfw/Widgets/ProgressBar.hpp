#ifndef GUI_PROGRESS_BAR_HPP
#define GUI_PROGRESS_BAR_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/Gfx/Elements/Text.hpp"
#include "sfw/Geometry.hpp"

namespace sfw
{

enum LabelPlacement
{
    LabelNone,   // Do no display the label
    LabelOver,   // Display the label over the progress bar
    LabelOutside // Display the label outside the progress bar
};

/**
 * This widget provides a horizontal progress bar.
 * Passive widget: cannot be focused or trigger event
 */
class ProgressBar: public Widget
{
public:
    /**
     * @param length: bar length bar in pixels (Horizontal or Vertical, according to orientation)
     * @param orientation: orientation of the progress bar (Horizontal or Vertical)
     * @param labelPlacement: where to place the label (XXX%)
     */
    ProgressBar(float length = 200.f, Orientation orientation = Horizontal, LabelPlacement labelPlacement = LabelOver);

    /// [0..100]
    ProgressBar* set(float value);
    float get() const;

private:
    void draw(const gfx::RenderContext& ctx) const override;
    // Callbacks
    void onThemeChanged() override;
    // Helpers
    void updateGeometry();

    Orientation m_orientation;
    float m_boxLength;
    LabelPlacement m_labelPlacement;
    float m_value;
    Box m_box;
    sf::Vertex m_bar[4];
    Text m_label;
};

} // namespace

#endif // GUI_PROGRESS_BAR_HPP
