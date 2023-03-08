#include "sfw/WidgetContainer.hpp"

#include "sfw/GUI-main.hpp"
#include "sfw/util/shims.hpp"

#include <string>
    using std::string, std::begin, std::end;
#include <charconv>
    using std::to_chars;
#include <utility>

namespace sfw
{

WidgetContainer::WidgetContainer():
    m_first(nullptr),
    m_last(nullptr),
    m_hover(nullptr),
    m_focus(nullptr)
{
}


WidgetContainer::~WidgetContainer()
{
    // Deallocate all widgets
    const Widget* widget = m_first;
    while (widget != nullptr)
    {
        const Widget* next = widget->m_next;
        delete widget;
        widget = next;
    }
}


Widget* WidgetContainer::add(Widget* widget, const std::string& name)
{
    widget->setParent(this);

    if (m_first == nullptr)
    {
        m_first = m_last = widget;
    }
    else
    {
        m_last->m_next = widget;
        widget->m_previous = m_last;
        m_last = widget;
    }

    if (GUI* Main = getMain(); Main != nullptr)
    {
        // Make the default hex for a more climactic debug experience...
        char abuf[17] = {0}; to_chars(abuf, end(abuf), (size_t)(void*)widget, 16);
        Main->remember(name.empty() ? string(abuf) : name, widget);
    }

    recomputeGeometry();

    return widget;
}


namespace {
//!!	Widget* lookup(const Widget* anchor, Widget* widget, const std::string& name)
}

Widget* WidgetContainer::addBefore(const Widget* anchor, Widget* widget, const std::string& name)
{
	return nullptr;
}

Widget* WidgetContainer::addBefore(const std::string& anchor, Widget* widget, const std::string& name)
{
	return nullptr;
}

Widget* WidgetContainer::addAfter(const Widget* anchor, Widget* widget, const std::string& name)
{
	return nullptr;
}

Widget* WidgetContainer::addAfter(const std::string& anchor, Widget* widget, const std::string& name)
{
	return nullptr;
}

} // namespace
