#include "sfw/Widgets/Button.hpp"
#include "sfw/Theme.hpp"
#include "sfw/util/shim/sfml.hpp"

#include <algorithm>
    using std::max;

namespace sfw
{

Button::Button(const std::string& text)
{
    onThemeChanged(); //!!Calling it this way is a temp. kludge (for DRY). Also: it has to happen before the rest of the init.
    setText(text); // Will resize, too
}

Button::Button(const std::string& text, std::function<void()> callback):
    Button(text)
{
    setCallback(callback);
}

Button::Button(const std::string& text, std::function<void(Button*)> callback):
    Button(text)
{
    setCallback(callback);
}


void Button::setText(const std::string& text)
{
    m_box.item().setString(/*sfw::*/stdstring_to_SFMLString(text));
    recomputeGeometry();
}

std::string Button::getText() const
{
    return /*sfw::*/SFMLString_to_stdstring(m_box.item().getString());
}


void Button::onThemeChanged()
{
    m_box.item().setFont(Theme::getFont());
    m_box.item().setCharacterSize((int)Theme::textSize);
    m_box.setSize((float)Theme::minWidgetWidth, Theme::getBoxHeight());
    recomputeGeometry();
}


void Button::recomputeGeometry()
{
    int fittingWidth = (int)(m_box.item().getLocalBounds().width + Theme::PADDING * 2 + Theme::borderSize * 2);
    int width = max(fittingWidth, Theme::minWidgetWidth);

    m_box.setSize((float)width, Theme::getBoxHeight());
    m_box.centerTextHorizontally(m_box.item());

    setSize(m_box.getSize());
}


void Button::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_box, sfml_renderstates);
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
        if (contains(sf::Vector2f(x, y)))
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
    if (contains({x, y}))
    {
        onUpdated();
    }
}


void Button::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter || key.code == sf::Keyboard::Space)
    {
        m_box.press();
        onUpdated();
    }
}


void Button::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
    {
        m_box.release();
    }
}

/*!!
Button* Button::setCallback(std::function<void(Button*)> callback)
{
    return (Button*) Widget::setCallback( [callback] (Widget* w) { callback( (Button*)w ); });
}
!!*/

} // namespace
