#include "sfw/GUI-main.hpp"
#include "sfw/Theme.hpp"

#include <iostream>
using namespace std;

namespace sfw
{

GUI::GUI(sf::RenderWindow& window, const sfw::Theme::Cfg& themeCfg):
    m_window(window),
    m_themeCfg(themeCfg)
{
    // "Officially" mark this object as the "Main" in the GUI Widget tree:
    m_parent = this;

    // Also register ourselves to our own widget registry, "just for completeness":
    widgets["/"] = this;

    setTheme(m_themeCfg);
    m_cursorType = sf::Cursor::Arrow; //!! Should also be done by setTeheme the future
}


void GUI::process(const sf::Event& event)
{
    switch (event.type)
    {
    case sf::Event::MouseMoved:
    {
        sf::Vector2f mouse = convertMousePosition(event.mouseMove.x, event.mouseMove.y);
        onMouseMoved(mouse.x, mouse.y);
        break;
    }

    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mouse = convertMousePosition(event.mouseButton.x, event.mouseButton.y);
            onMousePressed(mouse.x, mouse.y);
        }
        break;

    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mouse = convertMousePosition(event.mouseButton.x, event.mouseButton.y);
            onMouseReleased(mouse.x, mouse.y);
        }
        break;

    case sf::Event::MouseWheelScrolled:
        onMouseWheelMoved((int)event.mouseWheelScroll.delta);
        break;

    case sf::Event::KeyPressed:
        onKeyPressed(event.key);
        break;

    case sf::Event::KeyReleased:
        onKeyReleased(event.key);
        break;

    case sf::Event::TextEntered:
        onTextEntered(event.text.unicode);
        break;

    default:
        break;
    }
}


bool GUI::setTheme(const sfw::Theme::Cfg& themeCfg)
{
    if (&m_themeCfg != &themeCfg)
    {
        m_themeCfg = themeCfg;
    }

    if (!m_themeCfg.apply())
    {
        return false;
    }

    // Notify widgets of the change
    for (auto& [name, w] : widgets) //! std::map reorders alphabetically, which is a disadvantage here
    {
//cerr << name <<endl;
        const_cast<Widget*>(w)->onThemeChanged();
    }
    //!!This should be redundant: onThemeChanged would make widgets setSize() themselves
    //!!(+ do their own onResized) as needed, which (Widget::setSize) would in turn also
    //!!request the parent's recomputeGeometry)!
    //!!(NOTE: there are ample chances of infinite looping here (some of which I have duly
    //!!explored already...)!)
    //for (auto& [name, cw] : widgets)
    //{
    //    const_cast<Widget*>(cw)->onResized();
    //}

    return true;
}


void GUI::render()
{
    if (sfw::Theme::clearWindow)
    {
        m_window.clear(sfw::Theme::windowBgColor);
    }

    // Draw whatever we have, via our a top-level widget container ancestor
    draw(gfx::RenderContext{m_window, sf::RenderStates()}); //! function-style RenderContext(...) failed with CLANG
}


void GUI::remember(const string& name, Widget* widget)
{
    if (widgets.find(name) != widgets.end())
    {
        cerr << "- Warning: A widget with the name \"" << name << "\" has already been registered.\n"
             << "  Overriding...\n";
    }

    widgets[name] = widget;
}


Widget* GUI::recall(const std::string& name)
{
    auto widget_iter = widgets.find(name);
    if (widget_iter == widgets.end()) cerr << "- Warning: widget \"" << name << "\" not found!\n";
//if (widget_iter != widgets.end()) cerr << "recall: found widget: \"" << widget_iter->first << "\" == "<< (void*)widget_iter->second <<"\n";
    return widget_iter != widgets.end() ? widget_iter->second : (Widget*)nullptr;
}


sf::Vector2f GUI::convertMousePosition(int x, int y) const
{
    sf::Vector2f mouse = m_window.mapPixelToCoords(sf::Vector2i(x, y));
    mouse -= getPosition();
    return mouse;
}


void GUI::setMouseCursor(sf::Cursor::Type cursorType)
{
    if (cursorType != m_cursorType)
    {
        if (Theme::cursor.loadFromSystem(cursorType))
        {
            m_window.setMouseCursor(Theme::cursor);
            m_cursorType = cursorType;
        }
    }
}

} // namespace
