#include "sfw/Widgets/Button.hpp"
#include "sfw/Theme.hpp"

namespace gui
{

Button::Button(const sf::String& string)
{
    m_box.item().setFont(Theme::getFont());
    m_box.item().setCharacterSize((int)Theme::textSize);
    m_box.setSize((float)Theme::minWidgetWidth, Theme::getBoxHeight());
    setString(string);
    setSize(m_box.getSize());
}

Button::Button(const sf::String& string, std::function<void()> callback):
    Button(string)
{
    setCallback(callback);
}

Button::Button(const sf::String& string, std::function<void(Button*)> callback):
    Button(string)
{
    setCallback(callback);
}


void Button::setString(const sf::String& string)
{
    m_box.item().setString(string);

    // Recompute widget width
    int fittingWidth = (int)(m_box.item().getLocalBounds().width + Theme::PADDING * 2 + Theme::borderSize * 2);
    int width = std::max(fittingWidth, Theme::minWidgetWidth);
    m_box.setSize((float)width, Theme::getBoxHeight());
    m_box.centerTextHorizontally(m_box.item());
    setSize(m_box.getSize());
}


const sf::String& Button::getString() const
{
    return m_box.item().getString();
}


void Button::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    auto lstates = states;
    lstates.transform *= getTransform();
    target.draw(m_box, lstates);
}

// Callbacks -------------------------------------------------------------------

void Button::onStateChanged(WidgetState state)
{
    m_box.applyState(state);
}


void Button::onMouseMoved(float x, float y)
{
    if (getState() == WidgetState::Pressed)
    {
        if (containsPoint(sf::Vector2f(x, y)))
            m_box.press();
        else
            m_box.release();
    }
}


void Button::onMousePressed([[maybe_unused]] float x, [[maybe_unused]] float y)
{
    m_box.press();
}


void Button::onMouseReleased(float x, float y)
{
    if (containsPoint({x, y}))
    {
        triggerCallback();
    }
}


void Button::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
    {
        triggerCallback();
        m_box.press();
    }
}


void Button::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
    {
        m_box.release();
    }
}

Button* Button::setCallback(std::function<void(Button*)> callback)
{
    return (Button*) Widget::setCallback( [callback] (Widget* w) { callback( (Button*)w ); });
}

} // namespace
