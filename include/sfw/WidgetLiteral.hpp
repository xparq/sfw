//!!EXPERIMENTAL!!

#ifndef _KLDURYTBDIOUNYC560763B87HN85VNMB6I7_
#define _KLDURYTBDIOUNYC560763B87HN85VNMB6I7_

#include "sfw/Widget.hpp"
#include "sfw/WidgetPtr.hpp" // widget<WType>(name)

#include <string>
#include <string_view>
//#include <concepts>
//#include <expected>
//#include <type_traits> // invoke_result_t, is_void_v

#include <iostream>
#include <cassert>

namespace sfw
{
	template<std::size_t N, class WType = Widget> //!! DEFUNCT! :-/ Can't get WType from the _W literal op.!
		//!! Add comment about what *EXACTLY* N is here!

	struct WidgetNameLiteral //!! C++ bullshit adapter class to support (still only)
	{                  //!! user-defined string literal template parameters... :-/
		char literal_name[N+1] = {0};
		WType* wptr = nullptr;

		constexpr WidgetNameLiteral(char const(&cstr)[N])
		{
			//!!std::ranges::copy(cstr, literal_name);
			//!! - but my GCC (13.x) can't seem to do that yet, and also to spare an underutilized dependency:
			for (auto outp = literal_name; auto c : cstr) *outp++ = c;
#if 0 //!!C++: Couldn't make it compatible with this consteval check:
			if consteval {
				//!!...auto length = outp - literal_name;
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
#ifdef DEBUG
std::cerr << __FUNCTION__ << ": NOTE: Lookup implicitly via the last GUI manager instance.\n";
#endif
			wptr = widget<AnyWidget>(literal_name);
			return (AnyWidget*) wptr;
		}

		template <class AnyWidget>
		constexpr operator AnyWidget& ()
		// To support this syntax: ((TextBox&)"some name"_W).set("It works!")
		{
#ifdef DEBUG
std::cerr << __FUNCTION__ << ": NOTE: Lookup implicitly via the last GUI manager instance.\n";
#endif
			wptr = widget<AnyWidget>(literal_name);
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

/*!!C++: This is ALSO futile, as "name"_W.call([](SomeWidget* w){w->op();})
         is not enough to deduce the widget type from the lambda arg (w)! :-/
         (Note: the code below doesn't even try that; it's just a placeholder.)

		template <typename F>
		auto call(F&& f) //!!??C++: Or const F&? Or even just F?...
			-> std::expected<typename std::invoke_result_t<std::decay_t<decltype(f)>, WType*>, //!!??C++: Raw F is not enough: std::expected can't hold references! But it still compiles with just F...
				std::nullptr_t>
		{
			if (wptr) {
				//!!C++: This sad conditional is to avoid a compiler error if f is void:
				if constexpr (std::is_void_v<std::invoke_result_t<std::decay_t<decltype(f)>, WType*>>) { //!!??C++: Raw F is not enough: std::expected can't hold references! But it still compiles with just F...
					f(wptr);
					return {}; //! Not just `return;`: std::expected needs an init. list.
				} else {
					return f(wptr); //!!C++: Not forward(), because std::expected can't hold references...
							//!!     (Also, providing the correct templ. param for forward is yet more nuisance.)
				}
			}
			else
			{
				std::cerr << "- WARNING: Operation on NULL widget pointer ignored!"
#ifdef DEBUG
				          << " (Widget type: "<< typeid(W).name() <<")"
#endif
				          << std::endl;
				return std::unexpected(nullptr);
			}
		}
!!*/

	}; // class WidgetLiteral

	template<WidgetNameLiteral s> //!!C++: Parametrizing by another type like "..."_W<Button> is not possible. :-/
	constexpr auto operator""_W() { return s; };

} // namespace sfw

#endif // _KLDURYTBDIOUNYC560763B87HN85VNMB6I7_
