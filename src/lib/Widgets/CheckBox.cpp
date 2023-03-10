#include "sfw/Widgets/CheckBox.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace sfw
{

CheckBox::CheckBox(bool checked):
    m_box(Box::Input)
{
    float offset = Theme::PADDING + Theme::borderSize;
    float box_size = m_checkmark.getSize().x + offset * 2;
    m_box.setSize(box_size, box_size);
    m_checkmark.setPosition({offset, offset});
    check(checked);

    setSize(m_box.getSize());
}

CheckBox::CheckBox(std::function<void(CheckBox*)> callback, bool checked):
    CheckBox(checked)
{
    setCallback(callback);
}


bool CheckBox::isChecked() const
{
    return m_checked;
}


void CheckBox::check(bool checked)
{
    m_checked = checked;
}


void CheckBox::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_box, sfml_renderstates);
    if (m_checked)
        ctx.target.draw(m_checkmark, sfml_renderstates);
#ifdef DEBUG
//    Widget::draw_outline(ctx);
#endif
}

// callbacks -------------------------------------------------------------------

void CheckBox::onStateChanged(WidgetState state)
{
    m_box.applyState(state);
}


void CheckBox::onMouseReleased(float x, float y)
{
    if (containsPoint(sf::Vector2f(x, y)))
    {
        check(!m_checked);
        triggerCallback();
    }
}


void CheckBox::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Space)
    {
        check(!m_checked);
        triggerCallback();
    }
}


CheckBox* CheckBox::setCallback(std::function<void(CheckBox*)> callback)
{
    return (CheckBox*) Widget::setCallback( [callback] (Widget* w) { callback( (CheckBox*)w ); });
}

} // namespace
