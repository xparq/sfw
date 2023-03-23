#ifndef GUI_SLIDER_HPP
#define GUI_SLIDER_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/Geometry.hpp"

namespace sfw
{

/**
 * This widget provides a vertical or horizontal slider.
 * The slider is the classic widget for controlling a bounded value.
 * The callback is triggered when 'Return' key is pressed.
 */
class Slider: public Widget
{
public:
    Slider(float step = 10.f, float length = 200, Orientation orientation = Horizontal);

    float getStep() const;

    /**
     * Define the amount of units to change the slider when adjusting by drag and drop
     */
    Slider* setStep(float step);

    float getValue() const;

    Slider* setValue(float value);

    Slider* setCallback(std::function<void(Slider*)> callback);
    Slider* setCallback(std::function<void()> callback)         { return (Slider*) Widget::setCallback(callback); }

private:
    void draw(const gfx::RenderContext& ctx) const override;

    void updateHandlePosition();

    // Callbacks
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onMousePressed(float x, float y) override;
    void onMouseMoved(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onMouseWheelMoved(int delta) override;
    void onStateChanged(WidgetState state) override;
    void onThemeChanged() override;
    // Helpers
    void updateGeometry();
    float mouseToValue(float x, float y) const;

    Orientation m_orientation;
    float m_boxLength;
    float m_step;
    float m_value;
    Box m_groove;
    Box m_handle;
    sf::Vertex m_progression[4];
};

} // namespace

#endif // GUI_SLIDER_HPP
