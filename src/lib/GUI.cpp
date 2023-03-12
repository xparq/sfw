#include "sfw/GUI-main.hpp"
#include "sfw/Theme.hpp"

#include <system_error>
#include <iostream> // for printing errors
using namespace std;

namespace sfw
{

GUI::GUI(sf::RenderWindow& window, const sfw::Theme::Cfg& themeCfg):
    m_error(), // no error by default
    m_window(window),
    m_themeCfg(themeCfg)
{
    // "Officially" mark this object as the "Main" in the GUI Widget tree:
    m_parent = this;

    // Also register ourselves to our own widget registry:
    widgets["/"] = this;

    reset();
}


GUI::operator bool() { return !m_error; }


bool GUI::reset()
{
    m_error = std::error_code();

    setTheme(m_themeCfg); //! Should this be reset to some internal safe default instead?
                          //! And then should also even the widgets be deleted, perhaps? :-o
                          //! No. Better to destroy the entire GUI for that. And for a
                          //! blank slate a new GUI instance can always be created.
                          //! Which then means...: the current config should also NOT be zapped!

    m_cursorType = sf::Cursor::Arrow;

    return (bool)*this;
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
//traverseChildren([&](Widget* w) { cerr << w->getName() << "\n"; } );

    //!!Should be transactional, with no side-effects if failed!

    if (&m_themeCfg != &themeCfg)
    {
        m_themeCfg = themeCfg;
    }

    // Do this even if the config has not been changed, to allow calling from the ctor!
    if (!m_themeCfg.apply())
    {
        m_error = make_error_code(errc::invalid_argument); //!!Should actually be a custom one!
        cerr << "- ERROR: Failed to setup theme!\n";
        return false;
    }

    // Notify widgets of the change
    traverseChildren([](Widget* w) { w->onThemeChanged(); } );
        // Another reason to not use the widget registry map (besides "frugalism"
        // and that it's not even intended for this (i.e. it may not even contain
        // every widget!) that std::map reorders its content alphabetically,
        // which is pretty awkward here (e.g. for diagnostics).

    //! This would be redundant in the current model:
    //!traverseChildren([](Widget* w) { w->recomputeGeometry(); } );
    //! The onThemeChanged() call typically involves setSize() too, which
    //! in turn also calls parent->recomputeGeometry() (via Widget::setSize)!
    //! (NOTE: there are ample chances of infinite looping here too, some of
    //! which I've duly explored already...)

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


Widget* GUI::recall(const std::string& name) const
{
    auto widget_iter = widgets.find(name);
    if (widget_iter == widgets.end()) cerr << "- Warning: widget \"" << name << "\" not found!\n";
//if (widget_iter != widgets.end()) cerr << "recall: found widget: \"" << widget_iter->first << "\" == "<< (void*)widget_iter->second <<"\n";
    return widget_iter != widgets.end() ? widget_iter->second : (Widget*)nullptr;
}

string GUI::recall(const Widget* w) const
{
    for (const auto& [name, widget]: widgets)
    {
        if (widget == w) return name;
    }
    return "";
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
