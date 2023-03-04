#include "sfw/GUI-main.hpp"
#include "sfw/Theme.hpp"

#include <iostream>
#include <format>
using namespace std;

namespace sfw
{

GUI::GUI(sf::RenderWindow& window, const sfw::Theme::Cfg& themeCfg):
    m_window(window),
    m_themeCfg(themeCfg),
    m_cursorType(sf::Cursor::Arrow) //!! Might depend on the theme config in the future
{
    // "Officially" mark this object as the "Main" in the GUI Widget tree:
    m_parent = this;
    // Also register ourselves to our own widget registry, "just for completeness":
    widgets["/"] = this;

    setTheme(m_themeCfg);
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
        m_themeCfg = themeCfg; //!! Must be Copyable! (Not enforced yet...)
    }

    if (!m_themeCfg.apply())
    {
        return false;
    }

//!!Unfortunately, map reorders them alphabetically, which doesn't help much...

    for (auto& [name, w] : widgets) {
//cerr << format("[{}] ({}) -> onThemeChanged()", name, (void*)w) << "\n";
        const_cast<Widget*>(w)->onThemeChanged();
    }

    for (auto& [name, cw] : widgets) {
//cerr << format("[{}] ({}) -> recomputeGeometry()", name, (void*)w) << "\n";
        auto w = const_cast<Widget*>(cw);
//	w->setSize(w->getSize());
        w->recomputeGeometry();
    }

#if 0
    // "This one" is not part of the widget chain!
    onThemeChanged();

/*!!traverseChildren crashes on OptionsBox theme change currently!!!
    traverseChildren([](auto* w) {
cerr << format("[{}] -> {} ", "<no names here yet!>", (void*)w) << "\n";
        w->onThemeChanged();        
    });
!!*/


//!!...
int i = 0;
    for (Widget* w = getFirstWidget(); w != nullptr; w = w->m_next)
    {
//cerr << ++i << "\n";
        w->onThemeChanged();
    }

i = 0;
    for (Widget* w = getFirstWidget(); w != nullptr; w = w->m_next)
    {
//cerr << ++i << "\n";
        w->recomputeGeometry();
    }

    // "This one" is not part of the widget chain!
    recomputeGeometry();
#endif

    return true;
}


void GUI::render()
{
    if (sfw::Theme::clearWindow)
        m_window.clear(sfw::Theme::windowBgColor);

    // Draw whatever we have (via our ancestor, a top-level VBoxLayout)
    draw(gfx::RenderContext{m_window, sf::RenderStates()}); //! RenderContext(...) failed with CLANG
}


bool GUI::remember(const string& name, const Widget* widget)
{
    if (widgets.find(name) != widgets.end())
    {
        cerr << "- Warning: A widget with the name \"" << name << "\" has already been registered.\n";
        cerr << "  Overriding...\n";
//        return false;
    }

    widgets[name] = widget;
    return true;
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
