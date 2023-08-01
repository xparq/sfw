#ifndef SFW_INPUTWIDGET_HPP
#define SFW_INPUTWIDGET_HPP

#include "sfw/Widget.hpp"

namespace sfw
{

template <class W> class InputWidget : public Widget
/*==============================================================================
  This "CRTP utility class" is an intermediate level in the widget class
  hierarchy, solely to overcome some C++ limitations (i.e. to support templated
  functions that would otherwise be unable to deduce their "actual widget type"
  parameter (W)).

  NOTE:
    "Interactive" widgets (that can have user callbacks, support the update(...)
    API etc.) should derive from this class (rather than Widget directly).
===============================================================================*/
{
public:
	template <typename V> W* update(V value) {
		((W*)this)->set(value); // set() is not virtual, must downcast to find it! (Also, `W::set(...)` could be too rigid, I think.)
		                        //!!NOTE: C++ won't find W::set() just via `set(value);` (even if no Widget::set())! :-/

		return (W*)updated();   // Downcasting only for the proper return type.
	}

	// Trigger the onUpdated() callback, if the widget's content has changed,
	// and then reset it back to unchanged.
	//!! -> "notify..." (and "onUpdted" -> "EventHandler::invoke_callback(Updated)"
	//!!    Note though, that this is not just a generic "notify on event" function,
	//!!    but one specific to botk InputWidget and changing the widget value (the "changed" logic)
	//!!    Could still be a virtual override of a generic EventHandler::notify, but I don't feel
	//!!    OK with that.
	//!!    -> also Widget::onUpdated() -- soon to become EventHandler::invoke_callback!...
	W* updated()
	{
		if (!changed()) return (W*)this;

		if (!disabled()) onUpdated(); //!! The entire "callback-triggering thing" should probably
		                              //!! be replaced by a generic event-proc. mechanism, so that
		                              //!! a) it can be configured "orthogonally", and b) we'd have
		                              //!! one less decision to make here.

		changed(false);
			//!!?? We either clear the `changed` flag here, so that it won't get stuck (note:
			//!!?? set() isn't supposed to clear it, to allow batch updates!), and then it can't
			//!!?? be used to track an overarching "changed" state across these update callbacks,
			//!!?? ...or we don't clear it, but then who will (or what if forgotten in user code)?

		return (W*)this;
	}

	//!! KINDA DEPRECATED...:
	// Set callback for Update events
	// 1. Applies to interactive (input) widgets only.
	// 2. The callback is not called for volatile interim state changes
	//    (like typing/deleting chars to/from a text editor), but only when
	//    applying (committing) the changes as the new content of the widget.
    	W* setCallback(std::function<void(W*)> callback)
	{
		on(Event::Update, [callback] (Event::Handler* w) { callback((W*)w); } );
		return (W*)this;
	}

	W* setCallback(std::function<void()> callback)
	{
		on(Event::Update, callback);
		return (W*)this;
	}


	// The actual (derived) widgets should then provide specific overloads, as:
	// SomeWidget* setCallback(std::function<void(SomeWidget*)> callback)
	// {
	//     return (SomeWidget*) Widget::setCallback([callback] (Widget* w) { callback( (SomeWidget*)w ); });
	// }
	// SomeWidget* setCallback(std::function<void()> callback)
	// {
	//     return (SomeWidget*) Widget::setCallback(callback);
	// }
};


} // namespace

#endif // SFW_INPUTWIDGET_HPP
