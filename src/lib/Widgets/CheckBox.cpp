#include "sfw/Widgets/CheckBox.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace sfw
{

CheckBox::CheckBox(bool checked_state):
    m_box(Box::Input)
{
    set(checked_state);

    float offset = Theme::PADDING + Theme::borderSize;
    float box_size = m_checkmark.getSize().x + offset * 2;
    m_box.setSize(box_size, box_size);
    m_checkmark.setPosition({offset, offset});

    setSize(m_box.getSize());
}

CheckBox::CheckBox(std::function<void(CheckBox*)> callback, bool checked):
    CheckBox(checked)
{
    setCallback(callback);
}


CheckBox* CheckBox::set(bool checked)
{
    m_checked = checked;
    triggerCallback();
    return this;
}


void CheckBox::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_box, sfml_renderstates);
    if (checked())
        ctx.target.draw(m_checkmark, sfml_renderstates);
}


// callbacks -------------------------------------------------------------------

void CheckBox::onStateChanged(WidgetState state)
{
    m_box.applyState(state);
}


void CheckBox::onThemeChanged()
{
    //!!...
}


void CheckBox::onMouseReleased(float x, float y)
{
    if (containsPoint(sf::Vector2f(x, y)))
    {
        toggle(); // will call triggerCallback()
    }
}


void CheckBox::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Space)
    {
        toggle(); // will call triggerCallback()
    }
}


CheckBox* CheckBox::setCallback(std::function<void(CheckBox*)> callback)
{
    return (CheckBox*) Widget::setCallback( [callback] (Widget* w) { callback( (CheckBox*)w ); });
}

} // namespace
