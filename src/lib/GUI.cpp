#include "sfw/GUI-main.hpp"
#include "sfw/Theme.hpp"

//!! Stuff for clearing the bg. when not owning the entire window
//!! (Should be moved to the Renderer!)
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>

#include <charconv>
#include <system_error>
#include <iostream> // for printing errors/warnings
#include <cassert>
using namespace std;

namespace sfw
{

//----------------------------------------------------------------------------
GUI::GUI(sf::RenderWindow& window, const sfw::Theme::Cfg& themeCfg, bool own_the_window):
    m_error(), // no error by default
    m_window(window),
    m_own_the_window(own_the_window),
    m_themeCfg(themeCfg)
{
    // "Officially" mark this object as the "Main" in the GUI Widget tree:
    m_parent = this;

    // Also register ourselves to our own widget registry:
    widgets["/"] = this;

    reset();
}


//----------------------------------------------------------------------------
bool GUI::active()
{
    return !m_closed && !m_error;
}


//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
void GUI::close()
{
	m_closed = true;

	// Do we control the window, too (or just the widgets)?
	if (m_own_the_window) m_window.close();
}

//----------------------------------------------------------------------------
bool GUI::process(const sf::Event& event)
{
    if (!active()) return false;

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

    case sf::Event::Closed:
	close();
        return false;
	break;

    default:
        break;
    }

    return true;
}


//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
void GUI::render()
{
    if (!active()) return;

    if (sfw::Theme::clearBackground)
    {
        if (m_own_the_window)
        {
            m_window.clear(Theme::bgColor);
        }
        else
        {
            // Just clear the GUI rect!
            sf::RectangleShape bgRect(getSize());
            bgRect.setPosition(getPosition());
            bgRect.setFillColor(Theme::bgColor);
            bgRect.setOutlineThickness(0);
            //!!m_renderer.draw(bgRect);
            m_window.draw(bgRect);
	}
    }

    // Draw whatever we have, via our a top-level widget container ancestor
    /*m_renderer.*/draw(gfx::RenderContext{m_window, sf::RenderStates()}); //! function-style RenderContext(...) failed with CLANG
}


//----------------------------------------------------------------------------
bool GUI::remember(Widget* widget, string name, bool override_existing)
{
    if (name.empty())
    {
        // Make the default name hex, for a more climactic debug experience...:
        char defname[17] = {0}; to_chars(defname, std::end(defname), (size_t)(void*)widget, 16);
        name = string(defname);
        // Paranoid sanity-checking of this default name
        if (widgets[name] != nullptr)
        {
            cerr << "- Warning: Widget ["<<name<<"] has already been registered with this default name.\n";
            assert(widgets[name] == widget);
            return true;
        }
    }
    else if (auto other_it = widgets.find(name); other_it != widgets.end()) // Someone has this name aleady?
    {
        Widget* other = other_it->second;
        if (other == widget)
        {
            cerr << "- Warning: Repeated registration of widget as \"" << name << "\".\n";
            return true;
        }

        if (!override_existing)
        {
            cerr << "- Warning: Refusing to override the name (\"" << name << "\") of another widget.\n";
            return false;
        }
        else
        {
            cerr << "- Warning: Another widget has already been registered as \"" << name << "\".\n"
                 << "  Overriding...\n";

            // Forget the name of the other widget:
            auto result_of_forgetting = remember(other, "");
            assert(result_of_forgetting);

            // Fall through to assign `name` to `widget`...
        }
    }
    
    widgets[name] = widget;
    return true;
}

//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
sf::Vector2f GUI::convertMousePosition(int x, int y) const
{
    sf::Vector2f mouse = m_window.mapPixelToCoords(sf::Vector2i(x, y));
    mouse -= getPosition();
    return mouse;
}


//----------------------------------------------------------------------------
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
