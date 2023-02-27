#ifndef GUI_SLIDER_HPP
#define GUI_SLIDER_HPP

#include "sfw/Widget.hpp"
#include "sfw/Shapes/Box.hpp"
#include "sfw/Geometry.hpp"

namespace gui
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
    void setStep(float step);

    float getValue() const;

    void setValue(float value);

protected:
    // Callbacks
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onMousePressed(float x, float y) override;
    void onMouseMoved(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onMouseWheelMoved(int delta) override;
    void onStateChanged(WidgetState state) override;

private:
    void updateHandlePosition();

    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    Orientation m_orientation;
    float m_step;
    float m_value;
    Box m_groove;
    sf::Vertex m_progression[4];
    Box m_handle;
};

}

#endif // GUI_SLIDER_HPP
