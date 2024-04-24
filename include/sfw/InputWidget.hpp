#ifndef SFW_INPUTWIDGET_HPP
#define SFW_INPUTWIDGET_HPP

#include "sfw/Widget.hpp"

//!!#include <concepts>

namespace sfw
{

template <class W> class InputWidget : public Widget

//!! Unfortunately, since the widget arg. W actually derives from this class(!),
//!! not Widget, we can't just tack a type constraint here, as it won't compile:
//!!template <std::derived_from<Widget> W> class InputWidget : public Widget
//!! And, of course, the (self-referential) real thing can't even be expressed in C++:
//!!template <std::derived_from<InputWidget<W> W> class InputWidget : public Widget

/*****************************************************************************
  Base class of widgets that are intended to receive user input (data)
  
  (This is implemented as a CRTP class solely to overcome a C++ limitation to
  support templated functions that would otherwise be unable to deduce their
  "actual widget" template parameter (W).)

  Notes on the "input thing":

  - Input widgets (that can have user callbacks, support the update(...)
    API etc.) should derive from this class (rather than Widget directly).

  - Every input widget has a dedicated value of a corresponding type that
    the widget represents (models, stores, displays etc.).

    This value can be set via user input actions, or programmatically by the
    constructor, and a dedicated set(...) method (and its overloads, if any).
 
    The current value can be queried with the widget's get() method.
    (Unfortunately C++ doesn't support overloading on return types, so for
    outward type conversions, separate getXxx() methods may be defined.
    However, it's better to define the widget's data type explicitly, with
    conversions in both directions (converting ctors and "cast operators"),
    and then just use a single set/get method.)

  - Input widgets are typically configured to have callbacks to be invoked
    when the widget's value gets updated (with the possibility of still
    ignoring unimportant interim changes).
******************************************************************************/
{
public:
	InputWidget() :
		m_changed(true) // Initializing counts as a change on its own right!
		                // (So, this ensures proper update followups even if
				// an initial set() can't detect a change, e.g. because
				// there's no "invalid" value in the type, or because
				// set() ism't even called by the widget ctor (which is
				// no longer assumed/required -- exactly because of this
				// init safeguard...))!
	{}

	// Tracking the value of the widget
	// Derived real widgets will need to define getter/setters:
	// - set(V value); // Will call changed() as applicable
	// - V get();
	bool changed() const { return m_changed; }
	Widget* setChanged(bool newstate = true) { m_changed = newstate; return this; }
		//!!Rename this to a) not be so ambiguous with the above (const) one, and
		//!!b) be in line with other similar ones (like enable/disable etc.)!...
		//!!But change() here would be awkward -- even resorting to setChanged()
		//!!feels better, but then it's out-of-style (for the new API) again...)


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

		setChanged(false);
			//!!?? We either clear the `changed` flag here, so that it won't get stuck (note:
			//!!?? set() isn't supposed to clear it, to allow batch updates!), and then it can't
			//!!?? be used to track an overarching "changed" state across these update callbacks,
			//!!?? ...or we don't clear it, but then who will (or what if forgotten in user code)?

		return (W*)this;
	}

	//!!SHOULD BE DEPRECATED, in favor of on(...):
	// Set callback for the Updated events
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

	//!!This actually belongs to InputWidget (`protected` exactly for it to be visible
	//!!there), but not sure how a virtual would mix with that being a template!...
	//!!Also, it's not even just a dummy callback like the others, but a dispatcher! :-/
	//!!Needs cleanup...
	//!! -> Actually, as a generic "callback invoker", even up in EventHandler, like
	//!!    EventHandler::invoke_callback(which), could hit all the birds with one stone.

// Virtuals/Callbacks --------------------------------------------------------
protected:

	virtual void onUpdated() //!! -> "EventHandler::invoke_callback(Updated)", and "updated" -> "notify(Updated)" or sg...
	{
		using namespace Event::internal;
		
		auto callback = m_callbackMap[Event::Update];
		assert( std::holds_alternative<Callback_w>(callback) || std::holds_alternative<Callback_void>(callback) );
			//!!?? This may no longer be true after getting something from the map that may not have
			//!!?? been there! Seems to still work, so probably true, but make it explicit whether the
			//!!?? map would indeed return a false std::function, properly wrapped in optional<>!...

		if (std::holds_alternative<Callback_w>(callback) && std::get<Callback_w>(callback))
		{
			return (std::get<Callback_w>(callback).value()) (this);
		}
		else if (std::holds_alternative<Callback_void>(callback) && std::get<Callback_void>(callback))
		{
			return (std::get<Callback_void>(callback).value()) ();
		}
	}


private:
	bool m_changed = false;
};


} // namespace

#endif // SFW_INPUTWIDGET_HPP
