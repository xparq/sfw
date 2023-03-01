#include "sfw/Widgets/Slider.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

namespace gui
{

Slider::Slider(float step, float length, Orientation orientation):
    m_orientation(orientation),
    m_step(step),
    m_value(0),
    m_groove(Box::Input)
{
    // (Using unsigned short here (instead of e.g. just unsigned or size_t)
    // shuts up the "possible loss of data" warnings... Plain short would be
    // fine, too, but unsigned (where applicable) is cleaner conceptually.)
    unsigned short handleHeight = (unsigned short)Theme::getBoxHeight();
    unsigned short handleWidth = handleHeight / 2;
    unsigned short grooveHeight = (unsigned short)Theme::borderSize * 3;
    short grooveOffset = (short)(handleHeight - grooveHeight) / 2;

    if (orientation == Horizontal)
    {
        m_groove.setSize(length, grooveHeight);
        m_groove.setPosition(0, grooveOffset);
        m_handle.setSize(handleWidth, handleHeight);

        setSize(length, handleHeight);

        for (int i = 0; i < 4; ++i)
        {
            m_progression[i].color = Theme::windowBgColor;
            m_progression[i].position.x = m_groove.getPosition().x + Theme::borderSize;
            m_progression[i].position.y = m_groove.getPosition().y + Theme::borderSize;
        }
        m_progression[1].position.y += m_groove.getSize().y - Theme::borderSize * 2;
        m_progression[3].position.y += m_groove.getSize().y - Theme::borderSize * 2;
    }
    else
    {
        m_groove.setSize(grooveHeight, length);
        m_groove.setPosition(grooveOffset, 0);
        m_handle.setSize(handleHeight, handleWidth);

        setSize(handleHeight, length);

        for (int i = 0; i < 4; ++i)
        {
            m_progression[i].color = Theme::windowBgColor;
            m_progression[i].position.x = m_groove.getPosition().x + Theme::borderSize;
            m_progression[i].position.y = m_groove.getSize().y - Theme::borderSize;
        }
        m_progression[2].position.x += m_groove.getSize().x - Theme::borderSize * 2;
        m_progression[3].position.x += m_groove.getSize().x - Theme::borderSize * 2;
    }
    updateHandlePosition();
}


float Slider::getStep() const
{
    return m_step;
}


void Slider::setStep(float step)
{
    if (step > 0 && step < 100)
        m_step = step;
}


float Slider::getValue() const
{
    return m_value;
}


void Slider::setValue(float value)
{
    // Ensure value is in bounds
    if (value < 0)
        value = 0;
    else if (value > 100)
        value = 100;
    else
    {
        // Round value to the closest multiple of step
        float temp = floor(value + m_step / 2);
        value = temp - fmod(temp, m_step);
    }

    // If value has changed
    if (value != m_value)
    {
        m_value = value;
        triggerCallback();
        // Move the handle on the slider
        updateHandlePosition();
    }
}


void Slider::updateHandlePosition()
{
    if (m_orientation == Horizontal)
    {
        float max = getSize().x - m_handle.getSize().x - Theme::borderSize * 2;
        float x = max * m_value / 100 + Theme::borderSize;
        m_handle.setPosition(x, 0);
        m_progression[2].position.x = x;
        m_progression[3].position.x = x;
    }
    else
    {
        float max = getSize().y - m_handle.getSize().y - Theme::borderSize * 2;
        float reverse_value = 100.f - m_value;
        float y = max * reverse_value / 100 + (float)Theme::borderSize;
        m_handle.setPosition(0, y);
        m_progression[0].position.y = y;
        m_progression[2].position.y = y;
    }
}


void Slider::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_groove, sfml_renderstates);
    ctx.target.draw(m_progression, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
    ctx.target.draw(m_handle, sfml_renderstates);
#ifdef DEBUG
//    Widget::draw_outline({ctx.target, sfml_renderstates}); // Not the original, untransformed ctx.props!
#endif
}


// callbacks ------------------------------------------------------------------

void Slider::onKeyPressed(const sf::Event::KeyEvent& key)
{
    switch (key.code)
    {
    case sf::Keyboard::Left:
        setValue(m_value - m_step);
        break;
    case sf::Keyboard::Right:
        setValue(m_value + m_step);
        break;
    case sf::Keyboard::Home:
        setValue(0);
        break;
    case sf::Keyboard::End:
        setValue(100);
        break;
    default:
        break;
    }
}


void Slider::onMousePressed(float x, float y)
{
    if (m_orientation == Horizontal)
        setValue(100 * x / getSize().x);
    else
        setValue(100 - (100 * (y) / getSize().y));

    m_handle.press();
}


void Slider::onMouseMoved(float x, float y)
{
    if (getState() == WidgetState::Focused)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (m_orientation == Horizontal)
                setValue(100 * x / getSize().x);
            else
                setValue(100 - (100 * y / getSize().y));
        }
    }
    else if (m_handle.containsPoint(x, y))
    {
        m_handle.applyState(WidgetState::Hovered);
    }
    else
    {
        m_handle.applyState(WidgetState::Default);
    }
}


void Slider::onMouseReleased(float, float)
{
    m_handle.release();
}


void Slider::onMouseWheelMoved(int delta)
{
    setValue(m_value + (delta > 0 ? m_step : -m_step));
}


void Slider::onStateChanged(WidgetState state)
{
    if (state == WidgetState::Focused || state == WidgetState::Default)
    {
        m_handle.applyState(state);
    }
}

Slider* Slider::setCallback(std::function<void(Slider*)> callback)
{
    return (Slider*) Widget::setCallback( [callback] (Widget* w) { callback( (Slider*)w ); });
}

} // namespace
