#include "sfw/Widget.hpp"
#include "sfw/Layout.hpp"
#include <cmath>

namespace gui
{

Widget::Widget():
    m_parent(nullptr),
    m_previous(nullptr),
    m_next(nullptr),
    m_state(WidgetState::Default),
    m_selectable(true)
{
}


void Widget::setPosition(const sf::Vector2f& pos)
{
    m_position = pos;
    m_transform = sf::Transform(
        1, 0, pos.x, // translate x
        0, 1, pos.y, // translate y
        0, 0, 1
    );
}


void Widget::setPosition(float x, float y)
{
    setPosition(sf::Vector2f(x, y));
}


const sf::Vector2f& Widget::getPosition() const
{
    return m_position;
}


sf::Vector2f Widget::getAbsolutePosition() const
{
    sf::Vector2f position = m_position;
    for (Widget* parent = m_parent; parent != nullptr; parent = parent->m_parent)
    {
        position.x += parent->m_position.x;
        position.y += parent->m_position.y;
    }
    return position;
}


void Widget::setSize(const sf::Vector2f& size)
{
    m_size = size;
    if (m_parent != nullptr)
    {
        Widget* parent = m_parent;
        parent->recomputeGeometry();
    }
}


void Widget::setSize(float width, float height)
{
    setSize(sf::Vector2f(width, height));
}


const sf::Vector2f& Widget::getSize() const
{
    return m_size;
}


bool Widget::containsPoint(const sf::Vector2f& point) const
{
    return point.x > 0.f && point.x < m_size.x && point.y > 0.f && point.y < m_size.y;
}


bool Widget::isSelectable() const
{
    return m_selectable;
}


bool Widget::isFocused() const
{
    return m_state == WidgetState::Focused || m_state == WidgetState::Pressed;
}


void Widget::setSelectable(bool selectable)
{
    m_selectable = selectable;
}


void Widget::setCallback(std::function<void(void)> callback)
{
    m_callback = callback;
}


void Widget::triggerCallback()
{
    if (m_callback)
    {
        m_callback();
    }
}


void Widget::setParent(Layout* parent)
{
    m_parent = parent;
}


void Widget::setState(WidgetState state)
{
    m_state = state;
    onStateChanged(state);
}


WidgetState Widget::getState() const
{
    return m_state;
}


const sf::Transform& Widget::getTransform() const
{
    return m_transform;
}


void Widget::draw([[maybe_unused]] sf::RenderTarget& target, [[maybe_unused]] const sf::RenderStates& states) const
{
}

#ifdef DEBUG
/*!!??
namespace debug {
//!!??error: "Already has a body"... :-o
void Widget::draw([[maybe_unused]] sf::RenderTarget& target, [[maybe_unused]] const sf::RenderStates& states) const
{
	sf::RectangleShape r(sf::Vector2f(getSize().x, getSize().y));
	r.setPosition(getAbsolutePosition());
	r.setFillColor(sf::Color::Transparent);
	r.setOutlineThickness(2);
	r.setOutlineColor(sf::Color::Red);
	target.draw(r);
}
}
??!!*/
#endif


void Widget::setMouseCursor(sf::Cursor::Type cursor)
{
    // Propagate the request to the parent, until it reaches the top-level widget (Main),
    // which is the only one that can change the mouse cursor, via the SFML RenderWindow.
    if (m_parent)
        m_parent->setMouseCursor(cursor);
}


void Widget::centerText(sf::Text& text)
{
    sf::FloatRect r = text.getLocalBounds();
    text.setOrigin({r.left + std::round(r.width / 2.f), r.top + std::round(r.height / 2.f)});
    text.setPosition({m_size.x / 2, m_size.y / 2});
}

// callbacks -------------------------------------------------------------------

void Widget::onStateChanged(WidgetState) { }
void Widget::onMouseEnter() { }
void Widget::onMouseLeave() { }
void Widget::onMouseMoved(float, float) { }
void Widget::onMousePressed(float, float) { }
void Widget::onMouseReleased(float, float) { }
void Widget::onMouseWheelMoved(int) { }
void Widget::onKeyPressed(const sf::Event::KeyEvent&) { }
void Widget::onKeyReleased(const sf::Event::KeyEvent&) { }
void Widget::onTextEntered(uint32_t) { }

}
