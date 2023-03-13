#include "sfw/Layout.hpp"
#include "sfw/Theme.hpp"
#include "sfw/Gfx/Render.hpp"

#ifdef DEBUG
#   include "sfw/GUI-main.hpp"
#   include <iostream>
    using std::cerr, std::endl;

    namespace {
        static auto DEBUG_INSIGHT_KEY = sf::Keyboard::LAlt;
        static auto DEBUG_INSIGHT_KEY_PRESSED = false;
    }
#
#endif

namespace sfw
{

Layout::Layout():
    m_hover(nullptr),
    m_focus(nullptr)
{
}

// Overrides -----------------------------------------------------------------

void Layout::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();

#ifdef DEBUG
	if (DEBUG_INSIGHT_KEY_PRESSED && m_state == WidgetState::Hovered) {
		if (auto* root = getMain(); root) root->draw_outline(ctx, sf::Color::Yellow);
		draw_outline(ctx, sf::Color::White);
	}
#endif

    for (const Widget* widget = begin(); widget != end(); widget = next(widget))
    {
        widget->draw(gfx::RenderContext{ctx.target, sfml_renderstates});
#ifdef DEBUG
	if (DEBUG_INSIGHT_KEY_PRESSED && widget->m_state == WidgetState::Hovered) {
		if (auto* root = getMain(); root) {
			widget->draw_outline(ctx,
				const_cast<Widget*>(widget)->toLayout() ? sf::Color::White : sf::Color::Blue);
		}
	}
#endif
    }
}


void Layout::onStateChanged(WidgetState state)
{
    if (state == WidgetState::Default)
    {
        // Lost the focus, reset the focused state
        if (m_focus)
        {
            m_focus->setState(WidgetState::Default);
            m_focus = nullptr;
        }
    }
}


void Layout::onMouseMoved(float x, float y)
{
    // Pressed widgets still receive mouse move events even when not hovered if mouse is pressed
    if (m_focus && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        m_focus->onMouseMoved(x - m_focus->getPosition().x, y - m_focus->getPosition().y);
        if (!m_focus->containsPoint({x, y}))
        {
            m_hover = nullptr;
        }
    }
    else
    {
        // Loop over widgets
        for (Widget* widget = begin(); widget != end(); widget = next(widget))
        {
            // Convert mouse position to the widget's coordinate system
            sf::Vector2f mouse = sf::Vector2f(x, y) - widget->getPosition();
            if (widget->containsPoint(mouse))
            {
                if (m_hover != widget)
                {
                    // A new widget is hovered
                    if (m_hover)
                    {
                        m_hover->setState(m_focus == m_hover ? WidgetState::Focused : WidgetState::Default);
                        m_hover->onMouseLeave();
                    }

                    m_hover = widget;
                    // Don't set the Hovered state if the widget is already focused
                    if (m_hover != m_focus)
                    {
                        widget->setState(WidgetState::Hovered);
                    }
                    widget->onMouseEnter();
                }
                else
                {
                    widget->onMouseMoved(mouse.x, mouse.y);
                }
                return;
            }
        }
        // No widget hovered, remove hovered state
        if (m_hover)
        {
            m_hover->onMouseMoved(x, y);
            m_hover->setState(m_focus == m_hover ? WidgetState::Focused : WidgetState::Default);
            m_hover->onMouseLeave();
            m_hover = nullptr;
        }
    }
}


void Layout::onMousePressed(float x, float y)
{
    if (m_hover)
    {
        // Clicked widget takes focus
        if (m_focus != m_hover)
        {
            focusWidget(m_hover);
        }
        // Send event to widget
        sf::Vector2f mouse = sf::Vector2f(x, y) - m_hover->getPosition();
        m_hover->onMousePressed(mouse.x, mouse.y);
    }
    // User didn't click on a widget, remove focus state
    else if (m_focus)
    {
        m_focus->setState(WidgetState::Default);
        m_focus = nullptr;
    }
}


void Layout::onMouseReleased(float x, float y)
{
    if (m_focus)
    {
        // Send event to the focused widget
        sf::Vector2f mouse = sf::Vector2f(x, y) - m_focus->getPosition();
        m_focus->onMouseReleased(mouse.x, mouse.y);
        m_focus->setState(WidgetState::Focused);
    }
}


void Layout::onMouseLeave()
{
    // Propagate it to the hovered child, if any, and then stop hovering it
    if (m_hover)
    {
        m_hover->onMouseLeave();
        m_hover->setState(m_focus == m_hover ? WidgetState::Focused : WidgetState::Default);
        m_hover = nullptr;
    }
}


void Layout::onMouseWheelMoved(int delta)
{
    if (m_focus)
    {
        m_focus->onMouseWheelMoved(delta);
    }
}


void Layout::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key == Theme::nextWidgetKey)
    {
        if (!focusNextWidget())
            // Try to wrap around to the first widget then
            focusNextWidget();
    }
    else if (key == Theme::previousWidgetKey)
    {
        if (!focusPreviousWidget())
            // Try to wrap around to the last widget then
            focusPreviousWidget();
    }
    else if (m_focus)
    {
        m_focus->onKeyPressed(key);
    }

#ifdef DEBUG
    if (key.code == DEBUG_INSIGHT_KEY) DEBUG_INSIGHT_KEY_PRESSED = true;
#endif
}


void Layout::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (m_focus)
    {
        m_focus->onKeyReleased(key);
    }

#ifdef DEBUG
    if (key.code == DEBUG_INSIGHT_KEY) DEBUG_INSIGHT_KEY_PRESSED = false;
#endif
}


void Layout::onTextEntered(uint32_t unicode)
{
    if (m_focus)
    {
        m_focus->onTextEntered(unicode);
    }
}


bool Layout::focusWidget(Widget* widget)
{
    if (widget)
    {
        // If another widget was already focused, reset that
        if (m_focus && m_focus != widget)
        {
            m_focus->setState(WidgetState::Default);
            m_focus = nullptr;
        }
        // Apply focus to widget
        if (widget->isSelectable()) //!!??isFocusable()
        {
            m_focus = widget;
            widget->setState(WidgetState::Focused);
            return true;
        }
    }
    return false;
}


bool Layout::focusPreviousWidget()
{
    // If a sublayout is already focused
    if (m_focus && m_focus->toLayout())
    {
        if (m_focus->toLayout()->focusPreviousWidget())
            return true;
    }

    Widget* start = m_focus ? prev(m_focus) : rbegin();
    for (Widget* widget = start; widget != rend(); widget = prev(widget))
    {
        if (Layout* container = widget->toLayout(); container)
        {
            if (container->focusPreviousWidget())
            {
                focusWidget(container);
                return true;
            }
        }
        else if (focusWidget(widget))
        {
            return true;
        }
    }

    if (m_focus)
        m_focus->setState(WidgetState::Default);
    m_focus = nullptr;
    return false;
}


bool Layout::focusNextWidget()
{
    // If a sublayout is already focused
    if (m_focus && m_focus->toLayout())
    {
        if (m_focus->toLayout()->focusNextWidget())
            return true;
    }

    Widget* start = m_focus ? next(m_focus) : begin();
    for (Widget* widget = start; widget != end(); widget = next(widget))
    {
        if (Layout* container = widget->toLayout(); container)
        {
            if (container->focusNextWidget())
            {
                focusWidget(container);
                return true;
            }
        }
        else if (focusWidget(widget))
        {
            return true;
        }
    }

    if (m_focus)
        m_focus->setState(WidgetState::Default);
    m_focus = nullptr;
    return false;
}

} // namespace
