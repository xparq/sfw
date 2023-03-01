#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace sfw
{

template <class T>
ItemBox<T>::ItemBox(Box::Type type):
    Box(type)
{
    applyState(WidgetState::Default);
}

template <class T>
ItemBox<T>::ItemBox(const T& item, Box::Type type):
    Box(type),
    m_item(item)
{
    applyState(WidgetState::Default);
}

template <class T>
void ItemBox<T>::applyState(WidgetState state)
{
    Box::applyState(state);
    switch (state)
    {
    case WidgetState::Default:
        m_item.setFillColor(m_type == Click ? Theme::click.textColor : Theme::input.textColor);
        break;
    case WidgetState::Hovered:
        m_item.setFillColor(m_type == Click ? Theme::click.textColorHover : Theme::input.textColorHover);
        break;
    case WidgetState::Pressed:
    case WidgetState::Focused:
        m_item.setFillColor(m_type == Click ? Theme::click.textColorFocus : Theme::input.textColorFocus);
        break;
    }
}

template <class T>
void ItemBox<T>::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    Box::draw(target, states);
    target.draw(m_item, states);
}

template <class T>
void ItemBox<T>::onPress()
{
    m_item.move({0.f, 1.f});
}

template <class T>
void ItemBox<T>::onRelease()
{
    m_item.move({0.f, -1.f});
}

} // namespace
