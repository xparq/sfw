#ifndef _DM948576NDM948576NB398R7Y8UHJ4968VB_
#define _DM948576NDM948576NB398R7Y8UHJ4968VB_

#include "sfw/WidgetBase.hpp"
#include "sfw/WidgetBase.hpp"

#include <string>
#include <string_view>
#include <concepts>
#include <expected>
#include <utility> // declval
#include <type_traits> // invoke_result_t

#include <iostream>
#include <cassert> // strncpy

namespace sfw
{
	//!! Move this to a more appropriate C++ debullshitting place (e.g. sz/unilang?):
	namespace internal {
		template<typename MFP> struct deduce_class_from_mfp;
		template<typename Ret, class Class, typename... Args>
		struct deduce_class_from_mfp<Ret (Class::*)(Args...)>       { using type = Class; };
		template<typename Ret, class Class, typename... Args>
		struct deduce_class_from_mfp<Ret (Class::*)(Args...) const> { using type = Class; };

		template<typename MFP> struct deduce_rettype_from_mfp;
		template<typename Ret, class Class, typename... Args>
		struct deduce_rettype_from_mfp<Ret (Class::*)(Args...)>       { using type = Ret; };
		template<typename Ret, class Class, typename... Args>
		struct deduce_rettype_from_mfp<Ret (Class::*)(Args...) const> { using type = Ret; };

		template <typename ...Ts>
		struct last_arg { using type = typename decltype((std::type_identity<Ts>{}, ...))::type; };

	/*
		template <typename... Args> requires(sizeof...(Args) >= 1)
		decltype(auto) constexpr last_arg_cref(const Args&... args)
		{
		//!!C++ bullshit: This doesn't compile without that `static` (should be implied!)):
		//!!	constinit const static auto arg_cnt = sizeof...(Args);
		//!!	constinit const static auto last_arg = arg_cnt - 1;
		//!! Is there any real difference between constinit and constexpr here, BTW?
			constexpr auto arg_cnt = sizeof...(Args);
			constexpr auto last_arg = arg_cnt - 1;
		//	std::cerr << "last_arg_cref:arg_cnt = " << arg_cnt << '\n';
		//	std::cerr << "last_arg_cref:last_arg_ndx = " << last_arg << '\n';

			const auto argrefs = std::tie(args...);
		//	const std::tuple argvals{args...};
			return std::get<last_arg>(argrefs); //!!C++: Why wouldn't std::forward(...) compile here?! And how do I tell if I'd even need that at all??...
		}
	*/
	}


//----------------------------------------------------------------------------
// Widget proxy (lookup + access)...
//----------------------------------------------------------------------------

template <std::derived_from<Widget> W = Widget>
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
	//!! LIMITED "DEFAULT-MANAGER" FORM:
	WidgetPtr(std::string_view name): _ptr(_lookup(name)) { _warn_if_not_found(name); }
	//!! LEGACY "EXPLICIT-MANAGER" FORMS:
	WidgetPtr(std::string_view name, const auto* proxy): _ptr(_lookup(name, proxy))  { _warn_if_not_found(name); }
	WidgetPtr(std::string_view name, const auto& proxy): _ptr(_lookup(name, &proxy)) { _warn_if_not_found(name); }

	[[nodiscard]] operator bool() const { return !_is_null(); }

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
				<< " (Widget type: "<< typeid(W).name() <<")"
#endif
				<< std::endl;
			return std::unexpected(nullptr); //!!?? Return something more useful here?
		}
	}

	//----------------------------------------------------------------------
	// "Direct" pointer accesses -- only detected, but not prevented!
	// (Needed by some of the API-level accessors.)
	//----------------------------------------------------------------------

	[[nodiscard]] operator W* () const {
//std::cerr << "----- op W* ()\n";
		_err_null_deref();
		return _ptr;
	}

	[[nodiscard]] W& operator * () const {
//std::cerr << "----- op * ()\n";
		_err_null_deref();
		return *_ptr;
	}

	W* operator -> () { // It's always [[nodiscard]]...
//std::cerr << "----- op -> ()\n";
		_err_null_deref();
		return _ptr;
	}


protected:
	//!! Should probably use shared_ptr, eventually:
	W* const _ptr; // immutable

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

	W* _lookup(const std::string_view name, const Widget* proxy = nullptr) const {
		auto wptr = (W*) Widget::getWidget_proxy(name, proxy);
		//!! Alas, can't do it here, because _ptr may be uninitialized yet!
		//!! (See _ptr(_lookup...) at the ctors!)
		//!!_warn_if_not_found(name);
		return wptr;
	}

	bool _is_null() const { return _ptr == nullptr; }
//!!	bool _is_null() const { return _ptr == &_dummy_widget; } //!! No suitable "null widget" mechanism yet!
	void _warn_if_not_found(std::string_view name) const {
		if (!_ptr) std::cerr << "- WARNING: "
			<< "Widget \""<< name <<"\" not found!"
			<< " (WidgetPtr "<<*this<< " is now a NULL instance.)"
			<< std::endl;
	}
	void _err_null_deref() const {
		if (_is_null()) {
			std::cerr << "- ERROR: dereferencing NULL widget pointer!"
#ifdef DEBUG
				<< " (Widget type: "<< typeid(W).name() <<")"
#endif
				<< std::endl;
		}
	}
};

//!! No suitable "null widget" mechanism yet!
//!!template <std::derived_from<Widget> W> W WidgetPtr<W>::_dummy_widget;
	//!! Note: inline? And then wouldn't is_null() { return _ptr == &_dummy_widget; } be bogus?...


//!!C++: Needed for disambiguating some auto-conversions when resolving op<<...
template <class W>
decltype(auto) operator << (auto& out, const sfw::WidgetPtr<W>& wptr)
	{ return out <<"#"<<std::hex<<(void*)&wptr; }



//============================================================================
// Widget lookup - Legacy convenience API...
//============================================================================

//
// Find widget by name...
//
// The returned widget proxy ("pointer") blocks access (or converts to nullptr)
// if "name" was not found.
template <class W = Widget> WidgetPtr<W> getWidget(std::string_view name, const Widget* w) { return WidgetPtr<W>(name, w); }
template <class W = Widget> WidgetPtr<W> getWidget(std::string_view name, const Widget& w) { return WidgetPtr<W>(name, w); }

// Plus this one for convenience: the last created GUI instance will register itself as the default
// (singleton) widget manager, so we can talk to it, provided we know for sure there can't be
// another one in our app.
template <class W = Widget>
WidgetPtr<W> getWidget(std::string_view name) { return WidgetPtr( (W*)Widget::getWidget_proxy(name) ); }
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !!
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

	template<std::size_t N, class WType = Widget> //!! DEFUNCT! :-/ Can't get WType from the _W literal op.!
		//!! Add comment about what *EXACTLY* N is here!

	struct ConstString //!! C++ bullshit adapter class to support (still only
	{                  //!! user-defined!) string literal template parameters... :-/
		char literal_name[N+1] = {0};
		WType* wptr = nullptr;

		constexpr ConstString(char const(&cstr)[N])
		{
			//!! My GCC (13.2) doesn't know about this yet:
			//!!std::ranges::copy(cstr, literal_name);
			//!! So...:
			for (auto outp = literal_name; auto c : cstr) *outp++ = c;
#if 0 // I just couldn't make it consteval'ed:
			/*!! Can't constexpr this...!!*/ auto outp = literal_name;
			for (auto c : cstr) *outp++ = c;
			/*!! consetexpr...!!*/ auto length = outp - literal_name;
			if consteval {
				static_assert(length < N);
				static_assert(literal_name[length] == '\0');
			}
#endif
		}

		constexpr operator std::string      () const { return std::string(literal_name); }
		constexpr operator std::string_view () const { return std::string_view(literal_name); }

		template <class AnyWidget>
		constexpr operator AnyWidget* ()
		// To support this syntax: ((TextBox*)"some name"_W)->set("It works!")
		{
			wptr = getWidget<AnyWidget>(literal_name); //!! Only via the last gui instance yet! :-/
			return (AnyWidget*) wptr;
		}

		template <class AnyWidget>
		constexpr operator AnyWidget& ()
		// To support this syntax: ((TextBox&)"some name"_W).set("It works!")
		{
			wptr = getWidget<AnyWidget>(literal_name); //!! Only via the last gui instance yet! :-/
			return (AnyWidget&) *wptr;
		}
/*
		constexpr WType* operator -> ()
		{
//				std::cerr << "Woahh!!\n";
			wptr = getWidget<WType>(literal_name);
			return wptr;
		}
*/
		// And this one for: "some name"_W.call<&TextBox::set>("It works!")
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

			using W = internal::deduce_class_from_mfp<decltype(Method)>::type;
			static_assert(std::derived_from<W, Widget>);

			WidgetPtr<W> typed_wptr = getWidget<W>(literal_name);
			wptr = typed_wptr; // Set this too...

			if (typed_wptr) {
				auto&& method{Method}; // Yet some more cryptic C++ bullshit, sorry! ;)
				return std::invoke(method, *typed_wptr, std::forward<Args>(args)...);
			} else {
				std::cerr << "- WARNING: Operation on NULL widget pointer ignored!"
#ifdef DEBUG
					<< " (Widget type: "<< typeid(W).name() <<")"
#endif
					<< std::endl;
				return std::unexpected(nullptr); //!! Actually return something here! :)
			}
		}
	};
		
	template<ConstString s> //!! C++ forbids parametrizing by another type: no "..."_W<Button> :-/
	constexpr auto operator""_W() { return s; };

//----------------------------------------------------------------------------
// Universal widget manipulator proxy
//

//----------------------------------------------------------------------------
// Universal access (more optimal for modifiers than queries)
//
// Usage example:
//
//	with("widget name", &CheckBox::set, false);
//
template <typename Method, typename... Args>
auto with(const char* name, Method&& action, Args&&... args) //!! WON'T match string_view or string! :-/
	-> std::expected<
		decltype((std::declval<WidgetPtr<
				typename internal::deduce_class_from_mfp<Method>::type
			>>()->*action)(std::forward<Args>(args)...))
		, std::nullptr_t> //!!?? Return something useful here?!...
		//!! std::expected can't solve the main issue: safe implicit NOOP in case of failed lookup!
		//!! (Can't just conjure up a null widget here!...)
		//!! Perhaps with() should only support manipulator members only?
{
	static_assert(std::is_member_function_pointer_v<Method>);
	using W = internal::deduce_class_from_mfp<Method>::type;
	static_assert(std::derived_from<W, Widget>);

	if (WidgetPtr<W> wptr(name); wptr) {
		return std::invoke(action, *wptr, std::forward<Args>(args)...);
	} else {
		// The lookup in the WPtr ctor above is expected to have
		// already freaked out on null!...
		return std::unexpected(nullptr); //!!?? Actually return something useful here?
	}
}

// Slightly different syntax, with the method as template arg:
//
//	apply<&CheckBox::set>("widget name", new_value);
//
template<auto SpecificMethod, typename... Args>
decltype(auto) apply(const char* name, Args&&... args)
{
	using Method = decltype(SpecificMethod); //!! decltype's required for concepts? (as opposed to causing problems if used with `typename` args!) :-o
	return with<Method, Args...>(name, SpecificMethod,std::forward<Args>(args)...);
}

// Yet more different syntax, with a "widget name"_W literal also as a template arg:
//
//	apply_W<"widget name"_W, &CheckBox::set>(new_value);
//
template<auto ConstName, auto SpecificMethod, typename... Args> //!!C++: Vanilla string literals (e.g. const char*) can't be template args! :-/
decltype(auto) apply_W(Args&&... args)
{
	using Method = decltype(SpecificMethod); //!!C++: decltype's required for concepts? (as opposed to causing problems if used with `typename` args!) :-o
	return with<Method, Args...>(ConstName.literal_name, SpecificMethod, std::forward<Args>(args)...);
}


//----------------------------------------------------------------------------
// Queries, with a default (fallback) return value...
//
// Usage example:
//
//	with_default("findme", &TextBox::get, "");
//
template <typename Method, typename... Args, typename DefT>
auto with_default(const char* name, Method&& action, Args&&... args, DefT defval)
	-> decltype((std::declval<WidgetPtr<
				typename internal::deduce_class_from_mfp<Method>::type
			>>()->*action)(std::forward<Args>(args)...))
{
	static_assert(std::is_member_function_pointer_v<Method>);
	using W = internal::deduce_class_from_mfp<Method>::type;
	static_assert(std::derived_from<W, Widget>);
	// Alas, can't predeclare this before having to state the return value... :-/
	using MethodRetT = decltype((std::declval<WidgetPtr<
				typename internal::deduce_class_from_mfp<Method>::type
			>>()->*action)(std::forward<Args>(args)...));
	static_assert(std::is_convertible_v<DefT, MethodRetT>);

	if (WidgetPtr<W> wptr(name); wptr) {
		return std::invoke(action, *wptr, std::forward<Args>(args)...);
	} else {
		// The lookup in the WPtr ctor above is expected to have
		// already freaked out on null!...
		return defval; //!! return internal::last_arg_cref(args...);
	}
}


//----------------------------------------------------------------------------
// WIDGET MODIFIERS - MACRO VERSION...
//
// (Return values can be ignored, no-widget errors will be NOOPs.)
//
#define SFW_UPDATE_WIDGET(WType, name, Op, ...) \
	_modify_widget_macroed(#WType, #Op, WidgetPtr<WType>(name), &WType::Op __VA_OPT__(,) __VA_ARGS__)

//----------------------------------------------------------------------------
// WIDGET QUERIES - MACRO VERSION...
//
// Since the return values are used in this case, error handling requires
// active participation from the caller.
//

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This version requires the caller to explicitly check the validity of the
// result (instead of supplying a default in advance):
#define SFW_QUERY_WIDGET(WType, name, Op, ...) \
	_query_widget_macroed(#WType, #Op, WidgetPtr<WType>(name), &WType::Op __VA_OPT__(,) __VA_ARGS__)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This version requires the caller to supply a default in advance, which
// would be returned in case of errors ("or_else" "monadic" logic).
//
//!! - The last arg should be the "or else" default used in case of errors!
//!! - Also: Op could actually be optional, defaulting to ->get() -- but then
//!!   with a default result that would make the signature ambiguous! :-/
//!! So, for now, the signature has this awkward, but unambiguous ordering:
#define SFW_QUERY_WIDGET_WITH_DEFAULT(WType, name, Op, def, ...) \
	_query_widget_with_default_macroed(#WType, #Op, WidgetPtr<WType>(name), &WType::Op, def __VA_OPT__(,) __VA_ARGS__)
	

//----------------------------------------------------------------------------
//
//	Internal accessor variants used by the macros (which know & pass both
//	the widget type name and the method name!)...
//
//----------------------------------------------------------------------------

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <std::derived_from<Widget> W, typename Method, typename... Args>
auto _modify_widget_macroed(const char* wtype_name, const char* method_name,
		WidgetPtr<W> wptr, Method&& action, Args&&... args)
	->
	std::expected<
		decltype((std::declval<WidgetPtr<W>>()->*action)(std::forward<Args>(args)...))
		, std::nullptr_t> //!!...
{
	if (wptr) {
		return std::invoke(action, *wptr, std::forward<Args>(args)...);
	} else {
		std::cerr << "- WARNING: Attempted " << wtype_name<<"::"<<method_name << " on NULL ignored!" << std::endl;
		return std::unexpected(nullptr); //!! Actually return something here! :)
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <std::derived_from<Widget> W, typename Method, typename... Args>
[[nodiscard]] auto _query_widget_macroed(const char* wtype_name, const char* method_name,
		WidgetPtr<W> wptr, Method&& action, Args&&... args)
	-> std::expected<
		decltype((std::declval<WidgetPtr<W>>()->*action)(std::forward<Args>(args)...))
		, std::nullptr_t> //!!...

//!! Could use std::expected for the return value, but that would not solve the main issue:
//!! safe implicit NOOP in case of error (to support no explicit error checking at the call site)!
//!! And it can't, either: can't just conjure up some null-that-means-error value for every
//!! possible type! Perhaps with should only support manipulator members only!
{
	if (wptr) {
		return std::invoke(action, *wptr, std::forward<Args>(args)...);
	} else {
		std::cerr << "- WARNING: Attempted " << wtype_name<<"::"<<method_name << " on NULL ignored!" << std::endl;
		return std::unexpected(nullptr); //!! Actually return something here! :)
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <std::derived_from<Widget> W, typename Method, typename DefT, typename... Args>
[[nodiscard]] auto _query_widget_with_default_macroed(const char* wtype_name, const char* method_name,
		WidgetPtr<W> wptr, Method&& action, DefT defval, Args&&... args)
	-> decltype((std::declval<WidgetPtr<W>>()->*action)(std::forward<Args>(args)...))
{
	if (wptr) {
		return std::invoke(action, *wptr, std::forward<Args>(args)...);
	} else {
		std::cerr << "- WARNING: Attempted " << wtype_name<<"::"<<method_name << " on NULL ignored!" << std::endl;
		return defval;
	}
}


} // namespace

#endif // _DM948576NDM948576NB398R7Y8UHJ4968VB_
