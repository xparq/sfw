#include "sfw/Layout.hpp"
#include "sfw/Theme.hpp"
#include "sfw/Gfx/Render.hpp"
#include "sfw/util/shim/sfml.hpp" // for sf::Event::KeyEvent::==

#ifdef DEBUG
#   include "sfw/GUI-main.hpp"
#   include <iostream>
    using std::cerr, std::endl;

    namespace {
        static auto DEBUG_INSIGHT_KEY = sf::Keyboard::LAlt;
        static auto DEBUG_INSIGHT_KEY_PRESSED = false;
    }

#endif

namespace sfw
{

Layout::Layout():
    m_hoveredWidget(nullptr),
    m_focusedWidget(nullptr)
{
}

// Overrides -----------------------------------------------------------------

void Layout::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    gfx::RenderContext lctx{ctx.target, sfml_renderstates};

#ifdef DEBUG
	if (DEBUG_INSIGHT_KEY_PRESSED && getState() == WidgetState::Hovered) {
		if (auto* root = getMain(); root) {
//cerr << getAbsolutePosition().x << ", " << getAbsolutePosition().y << endl;
			root->draw_outline(ctx, sf::Color::Yellow);
		}
		draw_outline(lctx, sf::Color::White);
	}
#endif

    for (const Widget* widget = begin(); widget != end(); widget = next(widget))
    {
        widget->draw(lctx);
#ifdef DEBUG
        // "Dim" the widget rect. if disabled:
        if (widget->disabled()) {
			widget->draw_outline(lctx, sf::Color::Transparent, Theme::bgColor * sf::Color(200, 200, 200, 50));
        }
        // Draw an outline around the widget:
        if (DEBUG_INSIGHT_KEY_PRESSED && widget->getState() == WidgetState::Hovered) {
            if (auto* root = getMain(); root) {
                widget->draw_outline(lctx,
                    const_cast<Widget*>(widget)->toLayout() ? sf::Color::White : sf::Color::Blue);
            }
        }
#endif
    }
}


//----------------------------------------------------------------------------
void Layout::onStateChanged(WidgetState state)
{
    if (state == WidgetState::Default)
    {
        // Make sure no child is stuck focused
        if (m_focusedWidget)
        {
//cerr <<"Clearing focused child " << m_focusedWidget << (m_focusedWidget->toLayout() ? " (container!) ":"") <<endl;
            m_focusedWidget->setState(WidgetState::Default);
            m_focusedWidget = nullptr;
        }
    }
}

//----------------------------------------------------------------------------
void Layout::onMouseMoved(float x, float y)
{
    // Focused widgets still receive MouseMove events even when not hovered, when the mouse button is pressed
    if (m_focusedWidget && m_focusedWidget->enabled() && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        m_focusedWidget->onMouseMoved(x - m_focusedWidget->getPosition().x, y - m_focusedWidget->getPosition().y);
        // "Unmark" it as the currently hovered child if wandering off of it, though:
        if (!m_focusedWidget->contains({x, y}))
        {
            m_hoveredWidget = nullptr;
        }
        return;
    }

    for (Widget* widget = begin(); widget != end(); widget = next(widget))
    {
        if (!widget->enabled())
            continue;

        // Translate the mouse position to widget-local coordinates:
        sf::Vector2f mousePos = sf::Vector2f(x, y) - widget->getPosition();
        if (widget->contains(mousePos))
        {
            if (widget != m_hoveredWidget) // Hovered to another widget?
            {
                if (m_hoveredWidget)
                {
                    m_hoveredWidget->setState(m_focusedWidget == m_hoveredWidget ? WidgetState::Focused : WidgetState::Default);
                    m_hoveredWidget->onMouseLeave();
                }

                m_hoveredWidget = widget;
                // Set it to "Hovered" only if it's not also focused!
                if (m_hoveredWidget != m_focusedWidget)
                {
                    widget->setState(WidgetState::Hovered);
                }
                widget->onMouseEnter();
            }
            else
            {
                widget->onMouseMoved(mousePos.x, mousePos.y);
            }
            return;
        }
    }
    // No widget is hovered any longer, "unmark" the last one
    if (m_hoveredWidget)
    {
        if (m_hoveredWidget->enabled())
        {
            m_hoveredWidget->onMouseMoved(x, y);
            m_hoveredWidget->setState(m_focusedWidget == m_hoveredWidget ? WidgetState::Focused : WidgetState::Default);
            m_hoveredWidget->onMouseLeave();
        }
        m_hoveredWidget = nullptr;
    }
}


void Layout::onMousePressed(float x, float y)
{
    if (m_hoveredWidget && m_hoveredWidget->enabled())
    {
        // Try focusing the newly hovered (and clicked) widget...
        if (m_focusedWidget != m_hoveredWidget)
        {
            focusWidget(m_hoveredWidget);
                //! It might not have taken it, but never mind, carry on regardless...
        }
        // Send event to widget
        sf::Vector2f mouse = sf::Vector2f(x, y) - m_hoveredWidget->getPosition();
        m_hoveredWidget->onMousePressed(mouse.x, mouse.y);
    }
    else if (m_focusedWidget && m_focusedWidget->enabled()) // Clicked away from the focused widget?
    {
        m_focusedWidget->setState(WidgetState::Default); //!!->unfocus()!
        m_focusedWidget = nullptr;
    }
}


void Layout::onMouseReleased(float x, float y)
{
    if (m_focusedWidget && m_focusedWidget->enabled())
    {
        // Send event to the focused widget
        sf::Vector2f mouse = sf::Vector2f(x, y) - m_focusedWidget->getPosition();
        m_focusedWidget->onMouseReleased(mouse.x, mouse.y);
        m_focusedWidget->setState(WidgetState::Focused);
    }
}


void Layout::onMouseLeave()
{
    // Propagate it to the hovered child, if any, and then stop hovering it
    if (m_hoveredWidget && m_hoveredWidget->enabled())
    {
        m_hoveredWidget->onMouseLeave();
        m_hoveredWidget->setState(m_focusedWidget == m_hoveredWidget ? WidgetState::Focused : WidgetState::Default);
        m_hoveredWidget = nullptr;
    }
}


void Layout::onMouseWheelMoved(int delta)
{
    if (m_focusedWidget)
    {
        m_focusedWidget->onMouseWheelMoved(delta);
    }
}


void Layout::onKeyPressed(const sf::Event::KeyEvent& key)
{
    //!! Handle hotkeys (with bottom-up context bubbling, somehow inverting the top-down
    //!! logic of the event flow from Main -> container(s) -> leaf widget) -> #277
    //!! Currently Tab cycling is the only thing having hotkeys, and it's hardcoded, too:
    if (key == Theme::nextWidgetKey)
    {
        if (!focusNextWidget()) // false means no widget is focused now, and no suitable next w.
             focusNextWidget(); // Try to wrap around to the first widget then
        return; // Finish with this key, even if couldn't do anything with it!
    }
    else if (key == Theme::previousWidgetKey)
    {
        if (!focusPreviousWidget()) // No suitable prev. w., so no widget is focused now
             focusPreviousWidget(); // Try to wrap around to the last widget then
        return; // Finish with this key, even if couldn't do anything with it!
    }

    if (m_focusedWidget)
    {
        m_focusedWidget->onKeyPressed(key);
    }

#ifdef DEBUG
    if (key.code == DEBUG_INSIGHT_KEY) DEBUG_INSIGHT_KEY_PRESSED = true;
#endif
}


void Layout::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (m_focusedWidget)
    {
        m_focusedWidget->onKeyReleased(key);
    }

#ifdef DEBUG
    if (key.code == DEBUG_INSIGHT_KEY) DEBUG_INSIGHT_KEY_PRESSED = false;
#endif
}


void Layout::onTextEntered(char32_t unichar)
{
    if (m_focusedWidget)
    {
        m_focusedWidget->onTextEntered(unichar);
    }
}


bool Layout::focusWidget(Widget* widget)
{
//cerr << "focusWidget("<< widget <<") called" << (widget->toLayout() ? " (container!) ":"") <<endl;
    if (widget)
    {
        if (widget->focusable() && widget->enabled())
        {
            // If another widget was already focused, reset that
            if (m_focusedWidget && m_focusedWidget != widget)
            {
                m_focusedWidget->setState(WidgetState::Default);
                m_focusedWidget = nullptr;
            }
            m_focusedWidget = widget;
            widget->setState(WidgetState::Focused);
            return true;
        }
    }
    return false;
}


bool Layout::focusNextWidget()
{
    // If a sublayout has the focus, let it handle this there first, locally:
    if (m_focusedWidget && m_focusedWidget->toLayout())
    {
        if (m_focusedWidget->toLayout()->focusNextWidget())
            return true;
    }

    Widget* start = m_focusedWidget ? next(m_focusedWidget) : begin();
    for (Widget* widget = start; widget != end(); widget = next(widget))
    {
        if (Layout* container = widget->toLayout(); container)
        {
            if (container->focusNextWidget())
            {
                if (focusWidget(container))
                    return true;
            }
        }
        else if (focusWidget(widget))
        {
            return true;
        }
    }

    // No widget could be focused...
    if (m_focusedWidget)
        m_focusedWidget->setState(WidgetState::Default);
    m_focusedWidget = nullptr;
    return false;
}

bool Layout::focusPreviousWidget()
{
    // If a sublayout has the focus, let it handle this there first, locally:
    if (m_focusedWidget && m_focusedWidget->toLayout())
    {
        if (m_focusedWidget->toLayout()->focusPreviousWidget())
            return true;
    }

    Widget* start = m_focusedWidget ? prev(m_focusedWidget) : rbegin();
    for (Widget* widget = start; widget != rend(); widget = prev(widget))
    {
        if (Layout* container = widget->toLayout(); container)
        {
            if (container->focusPreviousWidget())
            {
                if (focusWidget(container))
                    return true;
            }
        }
        else if (focusWidget(widget))
        {
            return true;
        }
    }

    // No widget could be focused...
    if (m_focusedWidget)
        m_focusedWidget->setState(WidgetState::Default);
    m_focusedWidget = nullptr;
    return false;
}

} // namespace
