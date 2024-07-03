#include "sfw/WidgetContainer.hpp"

#include "sfw/GUI-main.hpp"
#include "sfw/adapter/sfml.hpp"

#include <cassert>
#include <string>
    using std::string, std::begin, std::end;
#include <charconv>
    using std::to_chars;
#include <utility>
#ifdef DEBUG
#include <iostream>
#endif


namespace sfw
{

WidgetContainer::WidgetContainer():
	m_first(nullptr),
	m_last(nullptr)
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

//----------------------------------------------------------------------------
//!!Replace this with an std-compliant interface + std::operations! (#162)
//!!And also provide (template?) variants for different f types!
void WidgetContainer::foreach(const std::function<void(Widget*)>& f)
{
	for (Widget* w = m_first; w; w = w->m_next)
		f(w);
}
void WidgetContainer::cforeach(const std::function<void(const Widget*)>& f) const
{
	for (const Widget* w = m_first; w; w = w->m_next)
		f(w);
}

bool WidgetContainer::foreachb(const std::function<bool(Widget*)>& f)
{
	for (Widget* w = m_first; w; w = w->m_next)
		if (!f(w)) return false;
	return true;
}
bool WidgetContainer::cforeachb(const std::function<bool(const Widget*)>& f) const
{
	for (const Widget* w = m_first; w; w = w->m_next)
		if (!f(w)) return false;
	return true;
}


//----------------------------------------------------------------------------
void WidgetContainer::traverse(const std::function<void(Widget*)>& f)
{
	foreach([&](auto* w) {
		f(w);
		w->traverse(f);
	});
}
void WidgetContainer::ctraverse(const std::function<void(const Widget*)>& f) const
{
	cforeach([&](const auto* w) {
		f(w);
		w->ctraverse(f);
	});
}


//----------------------------------------------------------------------------
bool WidgetContainer::is_child(const Widget* widget) const
{
	for (Widget* w = begin(); w != end(); w = next(w))
	{
		if (w == widget) return true;
	}
	return false;
}


//----------------------------------------------------------------------------
Widget* WidgetContainer::insert_after(Widget* anchor, Widget* widget, std::string_view name)
// This is the common workhorse procedure for all the other various add() methods.
// Does not check if anchor is in fact a local child!
{
	assert(widget);
	// Only freestanding widgets are to be attached:
	assert(widget->m_parent == nullptr && "Don't copy an already added widget!");
	assert(widget->m_next == nullptr);
	assert(widget->m_previous == nullptr);

	widget->setParent(this); // So far so good... ;)

	if (empty())
	{
		m_first = m_last = widget;
	}
	else if (anchor == nullptr) // add as first
	{
		// Just verifying !empty:
		assert(m_first);
		assert(m_last);
		// First is healthy?
		assert(m_first->m_previous == nullptr);
		assert(m_first->m_next);

		widget->m_next = m_first;
		m_first->m_previous = widget;
	}
	else if (anchor == m_last) // append (normal `add()`)
	{
		assert(anchor); // In case I'd feel like reshuffling the code... ;)
		// Just verifying !empty:
		assert(m_last);
		assert(m_first);
		// Last is healthy?
		//assert(m_last->m_previous); <-- anchor *can* also be the first!
		assert(m_last->m_next == nullptr);

		m_last->m_next = widget;
		widget->m_previous = m_last;

		m_last = widget;
	}
	else // So, we have both a previous and a next one to deal with,
	{    // but at least don't have to update this->first/last!...
		// In case I'd feel like reshuffling the code...:
		assert(anchor);
		assert(anchor != m_last);
		// Just verifying !empty:
		assert(m_last);
		assert(m_first);
		// Neighbors...?
		//assert (anchor->m_previous); <-- anchor *can* be the first!
		assert(anchor->m_next);

		widget->m_previous = anchor;
		widget->m_next = anchor->m_next;

		anchor->m_next = widget;
		widget->m_next->m_previous = widget;
	}


	// Register the widget globally, too
	if (GUI* Main = getMain(); Main)
	{
		Main->remember(widget, name); // Will assign default if name.empty()!
	}

	// Adjust the layout
	recomputeGeometry();

	return widget;
}


Widget* WidgetContainer::add(Widget* widget, std::string_view name)
{
	return insert_after(m_last, widget, name);
}


Widget* WidgetContainer::addAfter(Widget* anchor, Widget* widget, std::string_view name)
{
	if (anchor == nullptr)
	{
		// addLast
		return add(widget, name);
	}
	else if (is_child(anchor))
	{
		// link after anchor
		return insert_after(anchor, widget, name);
	}
	else return add(widget, name);
}

Widget* WidgetContainer::addAfter(std::string_view anchor_name, Widget* widget, std::string_view name)
{
	Widget* a = Widget::find_proxied(anchor_name); //!! See #322 for why not just this->widget...
	                                       //!!?? So, why exactly? I can't deduce the reason from #322 a year later! :-/
//!!??	Widget* a = this->widget(anchor_name);
	return a ? addAfter(a, widget, name) : add(widget, name);
}


Widget* WidgetContainer::addBefore(Widget* anchor, Widget* widget, std::string_view name)
{
	if (anchor == nullptr)
	{
        	// addFirst
        	return insert_after(nullptr, widget, name);
	}
	else if (is_child(anchor))
	{
		// link before anchor
		return insert_after(anchor->m_previous, widget, name);
	}
	else return add(widget, name);
}

Widget* WidgetContainer::addBefore(std::string_view anchor_name, Widget* widget, std::string_view name)
{
	Widget* a = Widget::find_proxied(anchor_name); //!! See #322 for why not just this->widget...
	                                         //!!?? So, why exactly? I can't deduce the reason from #322 a year later! :-/
//!!??	Widget* a = this->widget(anchor_name);
	return a ? addBefore(a, widget, name) : add(widget, name);
}

} // namespace sfw
