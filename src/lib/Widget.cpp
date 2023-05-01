#include "sfw/Widget.hpp"
#include "sfw/WidgetContainer.hpp"
#include "sfw/GUI-main.hpp"
#include "sfw/util/diagnostics.hpp"

#include <cassert>
#include <cmath>

#ifdef DEBUG
#
#  include <SFML/Graphics.hpp>
#
#endif

namespace sfw
{

Widget::Widget():
    m_parent(nullptr),
    m_previous(nullptr),
    m_next(nullptr),
    m_state(WidgetState::Default),
    m_focusable(true)
{
}


bool Widget::isRoot() const
{
    return getParent() == nullptr || getParent() == this;
}


bool Widget::isMain() const
{
#ifndef NDEBUG
    if (getParent() == this) assert(isRoot()); // not there in my MSVC yet: [[ assert: isRoot() ]]
#endif
    return getParent() == this;
}


Widget* Widget::getRoot() const
{
    return !isMain() && getParent() ? getParent()->getRoot() : const_cast<Widget*>(this); // Not const when returning itself...
}


GUI* Widget::getMain() const
{
    //!! Well, it's halfway between "undefined" and "bug" to call this on free-standing widgets...
    //!! But forbidding it would prevent adding children to them, criplling usability too much!
    //!!   assert(getParent() && getParent() != this ? getParent()->getRoot() : getParent());
    return (GUI*)(getParent() && getParent() != this ? getParent()->getRoot() : getParent());
}


void Widget::setName(const std::string& name)
{
    if (GUI* Main = getMain(); Main != nullptr)
    {
        Main->remember(this, name);
    }
}

std::string Widget::getName(Widget* widget) const
{
    if (GUI* Main = getMain(); Main != nullptr)
    {
        return Main->recall(widget ? widget : this);
    }
    return "";
}


Widget* Widget::getWidget(const std::string& name) const
{
    if (GUI* Main = getMain(); Main)
    {
        return Main->recall(name);
    }
    return nullptr;
}


Widget* Widget::setPosition(const sf::Vector2f& pos)
{
    m_position = {roundf(pos.x), roundf(pos.y)};
    m_transform = sf::Transform(
        1, 0, m_position.x, // translate x
        0, 1, m_position.y, // translate y
        0, 0, 1
    );

    return this;
}

Widget* Widget::setPosition(float x, float y)
{
    setPosition(sf::Vector2f(x, y));

    return this;
}


const sf::Vector2f& Widget::getPosition() const
{
    return m_position;
}


sf::Vector2f Widget::getAbsolutePosition() const
{
    sf::Vector2f position = m_position;

    for (const Widget* base = this; !base->isRoot();)
    {
	base = base->getParent();
        position.x += base->m_position.x;
        position.y += base->m_position.y;
    }

    return position;
}


void Widget::setSize(const sf::Vector2f& size)
{
    auto new_size = sf::Vector2f(roundf(size.x), roundf(size.y));
    if (m_size.x != new_size.x || m_size.y != new_size.y)
    {
        m_size = new_size;
        onResized();
        if (!isRoot()) getParent()->recomputeGeometry();
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


bool Widget::contains(const sf::Vector2f& point) const
{
    return point.x > 0.f && point.x < m_size.x && point.y > 0.f && point.y < m_size.y;
}


bool Widget::focused() const
{
    return m_state == WidgetState::Focused || m_state == WidgetState::Pressed;
}


void Widget::setFocusable(bool focusable)
{
    m_focusable = focusable;
}


bool Widget::focusable() const
{
    return m_focusable;
}


Widget* Widget::enable(bool state)
// To support the use case of mass-disabling/enabling a bunch of widgets,
// which typically involves recursive tree traversal via GUI::foreach(),
// some emerging inconsistencies must be explicitly addressed:
//
// - Some widgets will typically remain active, while others get disabled,
//   and those two sets may not align with container boundaries!
//   So, we can't just transitively disable/enable subtrees and call it a day.
//
// - Or, what should happen if a container gets disabled, but then a child of
//   it gets enabled specifically? Seems tempting to transitively re-enable
//   the container, but then all its other children that weren't explicitly
//   disabled would also get reenabled implicitly, which is not what should
//   happen.
//   Besides, unless all the children of a disabled container each get in
//   turn disabled too (recursively), there wouldn't even be any difference
//   between the child that was enabled specifically, and those that were
//   not, but are still supposed to become disabled implicitly...
//
//   - Or, if -- let's say -- a disabled parent container shouldn't get
//     reenabled by enabling a child of it, then how exactly should events
//     propagate down to that child across the disabled parent?
//
// Etc. So, to avoid such annoying inconsistencies and messy complications,
// containers just can't be disabled for now... (Which is also bad: making
// it impossible to a) disable an entire subtree of widgets with one move,
// and b) automatically preserve/restore individual widget disabled/enabled
// states within that subtree...)
{
    if (toLayout()) // Well, we are interested in Layouts actually, not WidgetContainers.
        return this;

    if (state) {
        if (m_state == WidgetState::Disabled)
        {
		    m_state = WidgetState::Default;
	        onStateChanged(m_state);
        }
    } else {
        if (m_state != WidgetState::Disabled)
        {
    		m_state = WidgetState::Disabled;
	        onStateChanged(m_state);
        }
    }
    return this;
}

bool Widget::enabled() const
{
	return m_state != WidgetState::Disabled;
}


Widget* Widget::setCallback(Callback callback)
{
    m_callback = callback;
    return this;
}

void Widget::setParent(WidgetContainer* parent)
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


// callbacks -----------------------------------------------------------------

void Widget::onUpdate()
{
    assert( std::holds_alternative<Callback_w>(m_callback) || std::holds_alternative<Callback_void>(m_callback) );

    if (disabled())
        return;

    if (std::holds_alternative<Callback_w>(m_callback) && std::get<Callback_w>(m_callback))
    {
        return (std::get<Callback_w>(m_callback).value()) (this);
    }
    else if (std::holds_alternative<Callback_void>(m_callback) && std::get<Callback_void>(m_callback))
    {
        return (std::get<Callback_void>(m_callback).value()) ();
    }
}

void Widget::onStateChanged(WidgetState) { }
void Widget::onMouseEnter() { }
void Widget::onMouseLeave() { }
void Widget::onMouseMoved(float, float) { }
void Widget::onMousePressed(float, float) { }
void Widget::onMouseReleased(float, float) { }
void Widget::onMouseWheelMoved(int) { }
void Widget::onKeyPressed(const sf::Event::KeyEvent&) { }
void Widget::onKeyReleased(const sf::Event::KeyEvent&) { }
void Widget::onTextEntered(char32_t) { }
void Widget::onThemeChanged() { }
void Widget::onResized() { }
void Widget::onTick() { }


// diagnostics ---------------------------------------------------------------

#ifdef DEBUG
void Widget::draw_outline([[maybe_unused]] const gfx::RenderContext& ctx, sf::Color outlinecolor, sf::Color fillcolor) const
{
	sf::RectangleShape r(sf::Vector2f(getSize().x, getSize().y));
	r.setPosition(getAbsolutePosition());
	r.setFillColor(fillcolor);
	r.setOutlineThickness(2);
	r.setOutlineColor(outlinecolor);
	ctx.target.draw(r);
}
#endif

} // namespace
