#include "sfw/Theme.hpp"
#include "sfw/util/shims.hpp" // std::string <-> sf::String conv.

#include <algorithm> // max

namespace sfw
{

template <class T>
OptionsBox<T>::OptionsBox():
    m_currentIndex((size_t)-1),
    m_box(Box::Input),
    m_arrowLeft(Arrow(Arrow::Left)),
    m_arrowRight(Arrow(Arrow::Right))
{
    onThemeChanged();
}

template <class T>
OptionsBox<T>::OptionsBox(std::function<void(OptionsBox<T>*)> callback):
    OptionsBox()
{
    setCallback(callback);    
}


template <class T>
auto OptionsBox<T>::addItem(const std::string& label, const T& value)
{
    m_items.push_back(Item(label, value));

    m_box.item().setString(/*sfw::*/stdstring_to_SFMLString(label));
    float width = m_box.item().getLocalBounds().width + Theme::getBoxHeight() * 2 + Theme::PADDING * 2;
    // Check if the box needs to be resized
    if (width > getSize().x)
    {
        m_box.setSize(width, (float)Theme::getBoxHeight());
        m_arrowRight.setPosition(width - (float)Theme::getBoxHeight(), 0.f);
        m_arrowRight.centerItem(m_arrowRight.item());
        setSize(m_box.getSize());
    }

    selectItem(m_items.size() - 1);
    return this;
}


template <class T>
auto OptionsBox<T>::selectItem(size_t item_index)
{
    if (item_index < m_items.size())
    {
        m_currentIndex = item_index;
        m_box.item().setString(/*sfw::*/stdstring_to_SFMLString(m_items[item_index].label));
        m_box.centerTextHorizontally(m_box.item());
    }
    return this;
}


template <class T>
const T& OptionsBox<T>::getSelectedValue() const
{
    return m_items[m_currentIndex].value;
}


template <class T>
size_t OptionsBox<T>::getSelectedIndex() const
{
    return m_currentIndex;
}


template <class T>
auto OptionsBox<T>::selectNext()
{
    if (m_items.size() > 1)
    {
        // Get next item index
        selectItem(m_currentIndex == (m_items.size() - 1) ? 0 : m_currentIndex + 1);
        triggerCallback();
    }
    return this;
}


template <class T>
auto OptionsBox<T>::selectPrevious()
{
    if (m_items.size() > 1)
    {
        // Get previous item index
        selectItem(m_currentIndex == 0 ? m_items.size() - 1 : m_currentIndex - 1);
        triggerCallback();
    }
}


template <class T>
OptionsBox<T>* OptionsBox<T>::setColor(const sf::Color& color)
{
    m_box.setItemColor(color);
    return this;
}


template <class T>
void OptionsBox<T>::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_box, sfml_renderstates);
    ctx.target.draw(m_arrowLeft, sfml_renderstates);
    ctx.target.draw(m_arrowRight, sfml_renderstates);
}



template <class T>
void OptionsBox<T>::updateArrowState(ItemBox<Arrow>& arrow, float x, float y)
{
    if (arrow.containsPoint(x, y))
    {
        if (getState() == WidgetState::Pressed)
            arrow.press();
        else
            arrow.applyState(WidgetState::Hovered);
    }
    else
    {
        arrow.applyState(isFocused() ? WidgetState::Focused : WidgetState::Default);
    }
}


// callbacks -------------------------------------------------------------------

template <class T>
void OptionsBox<T>::onThemeChanged()
{
    m_box.item().setFont(Theme::getFont());
    m_box.item().setCharacterSize((unsigned)Theme::textSize);

    // Update width to accomodate the widest element
    auto width = (float)Theme::minWidgetWidth;
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        m_box.item().setString(stdstring_to_SFMLString(m_items[i].label));
        width = std::max(width, m_box.item().getLocalBounds().width + Theme::getBoxHeight() * 2 + Theme::PADDING * 2);
    }
    m_box.setSize(width, (float)Theme::getBoxHeight());
    setSize(m_box.getSize());

    // Reset the current selection rolled all over in the loop above
    // (This will also center it, so the widget had to have been resized first!)
    selectItem(getSelectedIndex());

    // Left arrow
    m_arrowLeft.setSize(Theme::getBoxHeight(), Theme::getBoxHeight());
    m_arrowLeft.centerItem(m_arrowLeft.item());

    // Right arrow
    m_arrowRight.setSize(Theme::getBoxHeight(), Theme::getBoxHeight());
    m_arrowRight.setPosition(m_box.getSize().x - Theme::getBoxHeight(), 0);
    //!!WOW! Doing the same in reverse would make it fall apart spectacularly!! :-ooo
    //!!m_arrowRight.setPosition(m_box.getSize().x - Theme::getBoxHeight(), 0);
    //!!m_arrowRight.setSize(Theme::getBoxHeight(), Theme::getBoxHeight());

    m_arrowRight.centerItem(m_arrowRight.item());
}


template <class T>
void OptionsBox<T>::onStateChanged(WidgetState state)
{
    // Hovered state is handled in the onMouseMoved callback
    if (state == WidgetState::Default || state == WidgetState::Focused)
    {
        m_arrowLeft.applyState(state);
        m_arrowRight.applyState(state);
        m_box.applyState(state);
    }
}


template <class T>
void OptionsBox<T>::onMouseMoved(float x, float y)
{
    updateArrowState(m_arrowLeft, x, y);
    updateArrowState(m_arrowRight, x, y);
}


template <class T>
void OptionsBox<T>::onMousePressed(float x, float y)
{
    if (m_arrowLeft.containsPoint(x, y))
        m_arrowLeft.press();

    else if (m_arrowRight.containsPoint(x, y))
        m_arrowRight.press();
}


template <class T>
void OptionsBox<T>::onMouseReleased(float x, float y)
{
    if (m_arrowLeft.containsPoint(x, y))
    {
        m_arrowLeft.release();
        selectPrevious();
    }
    else if (m_arrowRight.containsPoint(x, y))
    {
        m_arrowRight.release();
        selectNext();
    }
}


template <class T>
void OptionsBox<T>::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Left)
    {
        selectPrevious();
        m_arrowLeft.press();
    }
    else if (key.code == sf::Keyboard::Right)
    {
        selectNext();
        m_arrowRight.press();
    }
}


template <class T>
void OptionsBox<T>::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Left)
    {
        m_arrowLeft.release();
    }
    else if (key.code == sf::Keyboard::Right)
    {
        m_arrowRight.release();
    }
}


template <class T>
OptionsBox<T>::Item::Item(const std::string& text, const T& val):
    label(text),
    value(val)
{
}


template <class T>
OptionsBox<T>* OptionsBox<T>::setCallback(std::function<void(OptionsBox<T>*)> callback)
{
    return (OptionsBox<T>*) Widget::setCallback( [callback](Widget* w) { callback( (OptionsBox*)w ); });
}

} // namespace
