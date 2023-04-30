#include "sfw/Widgets/Slider.hpp"
#include "sfw/Theme.hpp"
#include "sfw/util/diagnostics.hpp"

#include <cmath>
#include <algorithm>
    using std::min, std::max;

namespace sfw
{

Slider::Slider(float step, float length, Orientation orientation):
    m_orientation(orientation),
    m_boxLength(length),
    m_step(step),
    m_value(0),
    m_groove(Box::Input),
    m_handle(Box::Click)
{
    updateGeometry();
}


void Slider::updateGeometry()
{
    // (Using unsigned short here (instead of e.g. just unsigned or size_t)
    // shuts up the "possible loss of data" warnings... Plain short would be
    // fine, too, but unsigned (where applicable) is cleaner conceptually.)
    unsigned short handleHeight = (unsigned short)Theme::getBoxHeight();
    unsigned short handleWidth = handleHeight / 2;
    unsigned short grooveHeight = (unsigned short)Theme::borderSize * 3;
    short grooveOffset = (short)(handleHeight - grooveHeight) / 2;

    if (m_orientation == Horizontal)
    {
        m_groove.setSize(m_boxLength, grooveHeight);
        m_groove.setPosition(0, grooveOffset);
        m_handle.setSize(handleWidth, handleHeight);

        setSize(m_boxLength, handleHeight);

        for (int i = 0; i < 4; ++i)
        {
            m_progression[i].color = Theme::bgColor;
            m_progression[i].position.x = m_groove.getPosition().x + Theme::borderSize;
            m_progression[i].position.y = m_groove.getPosition().y + Theme::borderSize;
        }
        m_progression[1].position.y += m_groove.getSize().y - Theme::borderSize * 2;
        m_progression[3].position.y += m_groove.getSize().y - Theme::borderSize * 2;
    }
    else
    {
        m_groove.setSize(grooveHeight, m_boxLength);
        m_groove.setPosition(grooveOffset, 0);
        m_handle.setSize(handleHeight, handleWidth);

        setSize(handleHeight, m_boxLength);

        for (int i = 0; i < 4; ++i)
        {
            m_progression[i].color = Theme::bgColor;
            m_progression[i].position.x = m_groove.getPosition().x + Theme::borderSize;
            m_progression[i].position.y = m_groove.getPosition().y + Theme::borderSize;
        }
        m_progression[2].position.x += m_groove.getSize().x - Theme::borderSize * 2;
        m_progression[3].position.x += m_groove.getSize().x - Theme::borderSize * 2;
    }

    updateView();
}

void Slider::updateView()
{
    if (m_orientation == Horizontal)
    {
        float max = getSize().x - m_handle.getSize().x - Theme::borderSize * 2;
        float x = floor(max * m_value / 100 + Theme::borderSize);
        m_handle.setPosition(x, 0);
        m_progression[2].position.x = x;
        m_progression[3].position.x = x;
    }
    else
    {
        float max = getSize().y - m_handle.getSize().y - Theme::borderSize * 2;
        float reverse_value = 100.f - m_value;
        float y = floor(max * reverse_value / 100 + (float)Theme::borderSize);
        m_handle.setPosition(0, y);
        m_progression[0].position.y = y;
        m_progression[2].position.y = y;
    }
}

float Slider::mouseToValue(float x, float y) const
{
    float value;
    if (m_orientation == Horizontal)
        value = 100 * (x - Theme::borderSize) / (getSize().x - Theme::borderSize * 2);
    else
        value = 100 - (100 * (y - Theme::borderSize) / (getSize().y - Theme::borderSize * 2));

    value = max(0.f, value);
    value = min(100.f, value);
    return value;
}


Slider* Slider::setStep(float step)
{
    if (step > 0 && step < 100)
        m_step = step;
    return this;
}

float Slider::getStep() const
{
    return m_step;
}


Slider* Slider::set(float value)
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
        updateView();
        onUpdate();
    }
    return this;
}

float Slider::get() const
{
    return m_value;
}


void Slider::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_groove, sfml_renderstates);
    ctx.target.draw(m_progression, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
    ctx.target.draw(m_handle, sfml_renderstates);
}


// callbacks ------------------------------------------------------------------

void Slider::onKeyPressed(const sf::Event::KeyEvent& key)
{
    switch (key.code)
    {
    case sf::Keyboard::Left:
        if (m_orientation == Horizontal) set(m_value - m_step);
        break;
    case sf::Keyboard::Right:
        if (m_orientation == Horizontal) set(m_value + m_step);
        break;
    case sf::Keyboard::Down:
        if (m_orientation == Vertical) set(m_value - m_step);
        break;
    case sf::Keyboard::Up:
        if (m_orientation == Vertical) set(m_value + m_step);
        break;
    case sf::Keyboard::Home:
        set(0);
        break;
    case sf::Keyboard::End:
        set(100);
        break;
    default:
        break;
    }
}


void Slider::onMousePressed(float x, float y)
{
    set(mouseToValue(x, y));
    m_handle.press();
}


void Slider::onMouseMoved(float x, float y)
{
    if (getState() == WidgetState::Focused)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            set(mouseToValue(x, y));
/*
            if (m_orientation == Horizontal)
                set(100 * x / getSize().x);
            else
                set(100 - (100 * y / getSize().y));
*/
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
    set(m_value + (delta > 0 ? m_step : -m_step));
}


void Slider::onStateChanged(WidgetState state)
{
    if (state == WidgetState::Focused || state == WidgetState::Default)
    {
        m_handle.applyState(state);
    }
}

void Slider::onThemeChanged()
{
    updateGeometry();
}


Slider* Slider::setCallback(std::function<void(Slider*)> callback)
{
    return (Slider*) Widget::setCallback( [callback] (Widget* w) { callback( (Slider*)w ); });
}

} // namespace
