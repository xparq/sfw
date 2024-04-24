#ifndef _DM948576NDM948576NB398R7Y8UHJ4968VB_
#define _DM948576NDM948576NB398R7Y8UHJ4968VB_

#include "sfw/Widget.hpp"
#include "sfw/util/cpp/generic.hpp"

#include <string_view>
#include <concepts>
#include <expected>
#include <type_traits> // invoke_result_t, is_void_v, is_reference_v

#include <iostream>
#include <cassert>

#ifdef DEBUG
# include <string>
#endif


namespace sfw
{

//----------------------------------------------------------------------------
// Widget proxy (lookup + access)...
//----------------------------------------------------------------------------

//!!C++ bullshit: can't repeat the default param, if some other header already
//!! fw-declared this class, which happens to be the case currently...:
template <std::derived_from<Widget> W/*!! = Widget!!*/>
class WidgetPtr
/*
  NOTE:
	This "smart" widget pointer class can only protect against a subset
	of invalid widget accesses, i.e. where a widget identified by a name
	could not be looked up at all (by a WidgetPtr ctor).

	It can't help with dangling widget references!

	(IOW, an instance of this class will continute to exist happily after
	the real widget (looked up by a ctor) has been deleted by some other
	operation in the meantime, so any widget accesses via this "smart"
	pointer can only be guarded against cases where the widget was already
	not found by the initial lookup! Similarly, if a new named widget is
	added after a failed initial lookup for that same name, it will not
	be picked up; a "NULL WidgetPtr" will continue to _not_ know about
	(and forward requests to) it.)
*/
{
public:
	WidgetPtr(W* w): _ptr(w) { _warn_if_not_found("<name unknown; direct-from-pointer>"); }

	WidgetPtr(const WidgetPtr&) = delete;
	WidgetPtr(WidgetPtr&&) = delete;

	//!! NOTE: _ptr(dynamic_cast<W*>(...)) could be used below, but keeping
	//!!       RTTI (kinda) optional is still a priority.
	//!! LIMITED "DEFAULT CONTROLLER" FORM:
	WidgetPtr(std::string_view name): _ptr(_lookup(name)) { _warn_if_not_found(name); }
	//!! LEGACY "EXPLICIT CONTROLLER" FORMS:
	WidgetPtr(std::string_view name, const auto* proxy): _ptr(_lookup(name, proxy))  { _warn_if_not_found(name); }
	WidgetPtr(std::string_view name, const auto& proxy): _ptr(_lookup(name, &proxy)) { _warn_if_not_found(name); }

	[[nodiscard]] operator bool() const { return !_is_null(); }

/*!! Anything involving this `Method` trick is hopeless: can't handle overloads of `Method`!
	template <auto Method, typename... Args>
	auto call(Args&&... args)
		-> std::expected<
			typename internal::deduce_rettype_from_mfp<decltype(Method)>::type,
			std::nullptr_t
		>
	//!! std::expected for the return value would not solve the main issue:
	//!! safe implicit NOOP in case of error!
	//!! And it can't, either: can't just conjure up some null-for-error value for every
	//!! possible type! Perhaps this fn. should only support "manipulator" methods only!
	{
		static_assert(std::is_member_function_pointer_v<decltype(Method)>);

		using WReq = internal::deduce_class_from_mfp<decltype(Method)>::type;
		static_assert(std::derived_from<WReq, Widget>);

		if (_ptr) {
			auto&& method{Method}; // Yet some more cryptic C++ bullshit, sorry! ;)
			auto retyped_wptr = (WReq*)_ptr;
			return std::invoke(method, *retyped_wptr, std::forward<Args>(args)...);
		} else {
			std::cerr << "- WARNING: call(...) on NULL widget ptr. ignored!"
#ifdef DEBUG
				<< " (Widget name: \"" << _name << "\""
				<< ", type: "<< typeid(W).name() <<")"
#endif
				<< std::endl;
			return std::unexpected(nullptr); //!!?? Return something more useful here?
		}
	}
!!*/

//#ifndef _MSC_VER
//	auto call(auto&& f) //!!??C++: Or const F&? Or even just F?...
	                    //!!C++: that auto&& causes a template arg. error with MSVC at decltype(f) below! :-/
//#else
	template <typename F>
	auto call(F&& f) //!!??C++: Or const F&? Or even just F?...
//#endif
		-> std::expected<typename std::invoke_result_t<std::decay_t<decltype(f)>, W*>, //!!??C++: Raw F is not enough: std::expected can't hold references! But it still compiles with just F...
				std::nullptr_t>
	{
		//!C++: std::expected can't hold references, so:
		static_assert(!std::is_reference_v<std::invoke_result_t<std::decay_t<decltype(f)>, W*>>); //!!??C++: decltype(f) <-- would this be the same? (Didn't notice any difference.)
			//!!??C++: Is this too late here? The template already failed to match on the ref/val mismatch?? Or why can't I trigger this?
			//!!       Also, this should be just a warning, not an error!

		if (_ptr) {
			//!!C++: This sad conditional is to avoid a compiler error if f is void:
			if constexpr (std::is_void_v<std::invoke_result_t<std::decay_t<decltype(f)>, W*>>) { //!!??C++: Raw F is not enough: std::expected can't hold references! But it still compiles with just F...
				f(_ptr);
				return {}; //! Not just `return;`: std::expected needs an init. list.
			} else {
				return f(_ptr); //!!C++: Not forward(), because std::expected can't hold references...
				                //!!     (Also, providing the correct templ. param for forward is yet more nuisance.)
			}
		}
		else return std::unexpected(nullptr);
	}

/*!!C++: NOPE:
	// A synonym, if `do` could somehow sneak through the parsing...:
	template <auto Method, typename... Args>
	decltype(auto) do(Args&&... args) { return call(std::forward<Args>(args)...); }
!!*/

	//----------------------------------------------------------------------
	// "Direct" pointer accesses -- only detected, but not prevented!
	// (Needed by some of the API-level accessors.)
	//----------------------------------------------------------------------

	[[nodiscard]] operator W* () const {
//std::cerr << "----- op W* ()\n";
		_err_if_null_deref();
		return _ptr;
	}

	[[nodiscard]] W& operator * () const {
//std::cerr << "----- op * ()\n";
		_err_if_null_deref();
		return *_ptr;
	}

	W* operator -> () { // It's always [[nodiscard]]...
//std::cerr << "----- op -> ()\n";
		_err_if_null_deref();
		return _ptr;
	}
/*!!
	template <class AnyWidget>
	constexpr operator AnyWidget* ()
	{
std::cerr << "----- op AnyWidget* ()\n";
		_err_if_null_deref();
		return (AnyWidget*) _ptr;
	}

	template <class AnyWidget>
	constexpr operator AnyWidget& ()
	{
std::cerr << "----- op AnyWidget& ()\n";
		_err_if_null_deref();
		return (AnyWidget&) *_ptr;
	}
!!*/

protected:
#ifdef DEBUG
	mutable std::string _name{"???"};
		//! Must come before _ptr, for a timely init, as _ptr{_lookup(...)} will depend on it (in DEBUG mode)! :-o
#endif
	W* const _ptr; // immutable
		//!! Should probably use shared_ptr, eventually


/*!! No suitable "null widget" mechanism yet!
	class NullWidget : public Widget
	{
		NullWidget(...)
		{
		//!! Some special/extra setup for _NullWidget may be needed
		//!! so that it's safe to use in basically any scenario,
		//!! hence the dedicated NullWidget type...
		//!! But preferably a vanilla Widget should be usable as a dummy as is!
		//!! Alas, that would probably require default_constructible then,
		//!! otherwise the static dummy singleton can't be created uniformly! :-o
		//!!
		//!! HOWEVER: A "normal" default ctor needs an entirely different mode
		//!! of operation (and therefore setup) than a dedicated dummy state! :-o
		//!! (So then a dedicated ctor, too. :-/ )
		}
	};

	//!!static NullWidget _dummy_widget;
	//!! Or, if it can be guaranteed all throughout the entire lib that
	//!! every widget can be safe to use even if not a live participant
	//!! of the UI, then:
//!!	static W _dummy_widget;
!!*/

	W* _lookup(std::string_view name, const Widget* proxy = nullptr) const {
#ifdef DEBUG
		_name = std::string(name);
#endif
		//!!C++: _ptr is `const`, so can't initialize it here directly... :-/
		auto wptr = (W*) Widget::find_proxied(name, proxy);
		//!! Alas, can't call _warn_if_not_found here, because _ptr is not initialized yet (see _ptr(_lookup...) at the ctors!):
		//!!_warn_if_not_found(name);
		return wptr;
	}

	bool _is_null() const { return _ptr == nullptr; }
//!!	bool _is_null() const { return _ptr == &_dummy_widget; } //!! No suitable "null widget" mechanism yet!
	void _warn_if_not_found(std::string_view name) const {
		if (!_ptr) std::cerr << "- WARNING: "
			<< "Widget \""<< name <<"\" not found!"
			<< " (WidgetPtr "<<*this<< " has been set to NULL.)"
			<< std::endl;
	}
	void _err_if_null_deref() const {
		if (_is_null()) {
			std::cerr << "- ERROR: dereferencing NULL widget pointer!"
#ifdef DEBUG
				<< " (Widget name: \"" << _name << "\""
				<< ", type: "<< typeid(W).name() <<")"
#endif
				<< std::endl;
		}
	}
};

//!! No suitable "null widget" mechanism yet!
//!!template <std::derived_from<Widget> W> W WidgetPtr<W>::_dummy_widget;
	//!! Note: inline? And then wouldn't is_null() { return _ptr == &_dummy_widget; } be bogus?...


//!!C++: Needed for disambiguating some auto-conversions when resolving op<<...
template <std::derived_from<Widget> W>
auto& operator << (auto& out, const sfw::WidgetPtr<W>& wptr)
	{ auto save = out.flags(); out.setf(std::ios::hex); out <<"#"<<(void*)&wptr; out.setf(save); return out; } //!!C++: FFS!...


//----------------------------------------------------------------------------
// Widget lookup - via the default GUI controller
//
// Just a convenience forwarder to Widget::find(...).
//
// NOTE: The result of this can be (auto-converted) null, so NEVER USE DIRECTLY!
//       Use the null-guarded (and more task-specific)
//	 - [some_widget->]get_value<WidgetType>("name"),
//	 - [some_widget->]get_or<WidgetType>("name", default_value),
//	 - [some_widget->]get<WidgetType>("name", default_value) -> std::expected,
//	 - [some_widget->]set<WidgetType>("name", value),
//	 - [some_widget->]call<WidgetType>("name", [](auto* w){w->...})
//       API functions instead!
//
// The last created GUI instance will register itself as the default (singleton)
// widget manager, so unqualified queries will be dispatched to that. But note:
// this is intended for use cases where there's only one GUI instance in the app!
template <std::derived_from<Widget> W = Widget>
WidgetPtr<W> widget(std::string_view name)
{
	return Widget::find<W>(name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//!!
//!! std::expected can't handle references returned by F!
//!!
template <class W, typename F>
auto call(const char* name, F&& f) //!!??C++: Or const F&? Or even just F?...
	-> std::expected<typename std::invoke_result_t<std::decay_t<F>, W*>, //!!??C++: Raw F is not enough: std::expected can't hold references! But it still compiles with just F...
			std::nullptr_t>
{
	//!C++: std::expected can't hold references, so:
	static_assert(!std::is_reference_v<std::invoke_result_t<std::decay_t<F>, W*>>); //!!??C++: decltype(f) <-- would this be the same? (Didn't notice any difference.)
		//!!??C++: Is this too late here? The template already failed to match on the ref/val mismatch?? Or why can't I trigger this?
		//!!       Also, this should be just a warning, not an error!

	if (auto w = (W*)Widget::find_proxied(name); w) {
		//!!C++: This sad conditional is to avoid a compiler error if f is void:
		if constexpr (std::is_void_v<std::invoke_result_t<std::decay_t<F>, W*>>) { //!!??C++: Raw F is not enough: std::expected can't hold references! But it still compiles with just F...
			f(w);
			return {}; //! Not just `return;`: std::expected needs an init. list.
		} else {
			return f(w); //!!C++: Not forward(), because std::expected can't hold references...
			             //!!     (Also, providing the correct templ. param for forward is yet more nuisance.)
		}
	}

	return std::unexpected(nullptr);
}
/*!!C++: Unfortunately, can't make this just fowrard to a static Widget::call,
     so Widget->call and Widget::call could be nicely next to each other,
     because it's impossible to have them both there with the same name. :-/
template <std::derived_from<Widget> W, typename F>
decltype(auto) call(const char* name, F&& f) //!!?? Or const F&? Or even just F?...
{
	return Widget::call<W>(name, std::forward<F>(f)); //!!?? Or just (name, f)?
}
!!*/



//============================================================================
// GUARDED WIDGET ACCESS
//
// A widget is looked up by name, for invoking a method (with a set of
// parameters) on it.
// If the widget is not found, an error is logged/returned, and the method
// call is denied.
//
// The methods are categorized as either "modifiers" or "queries".
//
// The return value of modifiers can typically be ignored, so they can be
// called without explicit error checking: they will just do nothing (but
// log the error).
//
// Queries, OTOH, require active error-awareness from the caller, which
// either means explicit error checking, or supplying a fallback default
// result in advance (which will be returned in case of errors).
//
// They are also available both as "raw" free function, with somewhat awkward
// usage syntax, due to C++ limitations, and as macros, without the clutter
// and redundancy, but being old-school with clunky ergonomics (plus the
// usual macro issues -- which are basically negligible here, actually).
//============================================================================


/*!!C++ bullshit:
//
// Freestanding "Find widget by name" API functions
// (Workaround for C++ parsing issues with a templated Widget::getWidget(...); see below!)
//
// The returned widget proxy ("pointer") blocks access (or converts to nullptr)
// if "name" was not found.
template <class W = Widget> WidgetPtr<W> getWidget(std::string_view name* w) { return WidgetPtr<W>(name, w); }
template <class W = Widget> WidgetPtr<W> getWidget(std::string_view name, const Widget& w) { return WidgetPtr<W>(name, w); }
!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !! Why aren't these above just Widget:: (or GUI::) members?
  !! Well, this remnant, removed from the class (as (part) of #322) may help
  !! illuminate it:
  !!
	Widget* getWidget(const std::string& name) const;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!
	//!! JESUS CHRIST, C++, WTF?! :-ooo Get your shit together! ;) See #322...
	//!! Not even this sad workaround can work (except in MSVC, amen to that...)!
	//!! See the freestanding getWidget() functions after this class for the current
	//!! alternative solution.
	//!!
	//!!	template <class W = Widget>
	//!!	W* getWidget(const std::string& name) const { return (W*)getWidget_internal(name); }
	//!!		//! Would be nice to have it templated directly, but that would introduce
	//!!		//! a nasty cross-dependency on the top-level GUI stuff.
	//!!		Widget* getWidget_internal(const std::string& name) const;
	//!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


//----------------------------------------------------------------------------
// Returns std::expected, to be checked explicitly by the caller
template <std::derived_from<Widget> W, typename... Args>
decltype(auto) set(const char* name, Args&&... args)
{
	return call<W>(name, [&](W* w) { return w->set(std::forward<Args>(args)...); });
}

template <std::derived_from<Widget> W, typename... Args>
decltype(auto) update(const char* name, Args&&... args)
{
	return call<W>(name, [&](W* w) { return w->update(std::forward<Args>(args)...); });
}


//----------------------------------------------------------------------------
// Returns std::expected, to be checked explicitly by the caller
//
// Note: using the generic form (`get`) for returning std::expected (rather
// than values directly) to optimize for C++23 monadic `.or_else(...)` chaining
// (which should eventually make the `get_or` form (see below!) obsolete).
//
//!!
//!! Might still be too intrusive, in case parametrized normal getters
//!! (and setters, too, for that matter) should come into vogue (with which
//!! this would then conflict in style even if the specific type of `name`
//!! wouldn't be used and cause actual syntax errors)!
//!!
template <std::derived_from<Widget> W>
[[nodiscard]] auto get(const char* name)
{
	return call<W>(name, [](W* w) { return w->get(); });
}

//----------------------------------------------------------------------------
// Returns the widget's content if found, or a default (fallback) value if not
//
// Note: Should be deprecated if/when the C++23 monadic `.or_else(...)` form
// becomes the dominant style!
template <std::derived_from<Widget> W, typename TRet>
[[nodiscard]] auto get_or(const char* name, TRet defval) //!! std::expected's or_else is still too new (e.g. it's not there in my MSVC either)
{
	auto result = call<W>(name, [](W* w) { return w->get(); });
	return result ? result.value() : defval;
}
//----------------------------------------------------------------------------
// Plus a convenience synonym of get_or (as yet another `get` overload)
//
// Note: Should be deprecated if/when the C++23 monadic `.or_else(...)` form
// becomes the dominant style!
template <std::derived_from<Widget> W, typename TRet>
[[nodiscard]] auto get(const char* name, TRet defval) { return get_or<W, TRet>(name, std::forward<decltype(defval)>(defval)); }

//----------------------------------------------------------------------------
//! Only use this flavor if you're ABSOLUTELY sure that `name` does exist,
//! otherwise that .value() would throw bad_expected_access! (Well you may just
//! catch that one, too -- but that kinda defeats the purpose of using this in
//! the first place.)
//!
//!!??C++:
//!! std::expected can't handle references returned by get(), so no point in using
//!! `decltype(auto)` here, right? Also, call() itself already decays it anyway...
//!! Also:
//!! Since .value() returns a reference, MSVC warns about "returning address of
//!! local variable or temporary"... Is this a matter of copy elision? It's way
//!! too shaky, nonetheless, so better not trying to be reference-friendly here!
//!! No decltype(auto), just auto then...
template <std::derived_from<Widget> W>
[[nodiscard]] auto get_v(const char* name)
{
	return call<W>(name, [](W* w) { return w->get(); }).value();
	//!!??C++: NOPE (trying to forward references...):
	//!!return std::forward<std::invoke_result_t<???>(get<W>(name))>(get<W>(name).value());
}


//----------------------------------------------------------------------------
// MACRO VERSIONS...
//
//	some_widget->SFW_CALL(Button, "name", setText, "Found!"); // Proxied via some_widget...
//	SFW_CALL(Button, "name", setText, "Found!"); // ...or via the default (last) GUI controller
//
// Notes:
//
//	A widget-not-found error will be a NOOP.
//	Return values can be ignored.
//
// 	Captures everything by ref. to support the most common cases (because
//	the caller will have no way to adjust the capturing behavior of the lambda
//	for any variables they might need to pass).
//
//	(The unified macro syntax works thanks to having both a freestanding
//	sfw::call and Widget->call, with the same signature.)
//
#define SFW_CALL(W, name, Op, ...) \
	call<W>(name, [&](auto* _w_p_t_r_) { return _w_p_t_r_->Op(__VA_ARGS__); })
		// The funny arg. name is to avoid accidentally shadowing captured variables!

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Just an alternative syntax for sfw::set (or Widget->set):
#define SFW_SET(W, name, ...) \
	SFW_CALL(W, name, set, __VA_ARGS__)


} // namespace

#endif // _DM948576NDM948576NB398R7Y8UHJ4968VB_
