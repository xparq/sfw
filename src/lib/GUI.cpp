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

    // This one is not part of the widget chain!
    onThemeChanged();

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

    recomputeGeometry();

i = 0;
    for (auto& [name, widget] : widgets)
    {
cerr << format("{}: [{}] -> {} ", ++i, name, reinterpret_cast<size_t>((void*)widget)) << "\n";
    }

    return true;
}


void GUI::render()
{
    if (sfw::Theme::clearWindow)
        m_window.clear(sfw::Theme::windowBgColor);

    // Draw whatever we have (via our ancestor, a top-level VBoxLayout)
    draw(gfx::RenderContext{m_window, sf::RenderStates()}); //! RenderContext(...) failed with CLANG
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
