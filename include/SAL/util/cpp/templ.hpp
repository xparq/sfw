﻿#ifndef _DIOUNYWDIURBT570398FNY387NF8CU6DF344_
#define _DIOUNYWDIURBT570398FNY387NF8CU6DF344_


#include <utility> // declval
#include <type_traits> // invoke_result_t, type_identity, remove_reference


namespace internal { //!! RENAME!

	//!!C++: I think this would still fail with typedefed class names! :-/
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

	template <typename F, typename... Args>
	struct deduce_rettype { using type = std::invoke_result_t<F, Args...>; };

	template <typename ...Ts>
	struct last_arg { using type = typename decltype((std::type_identity<Ts>{}, ...))::type; };

/*
	template <typename... Args> requires(sizeof...(Args) >= 1)
	decltype(auto) constexpr last_arg_cref(const Args&... args)
	{
	//!!C++: This doesn't compile without that `static` (should be implied!)):
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

	// https://stackoverflow.com/questions/39816779/check-if-type-is-defined#comment133524396_74039411
	template<typename T, auto = []{}>
	struct type_defined: std::bool_constant<requires { sizeof(T); }> {};
		// Usage: static_assert(type_defined<name>())
		// https://stackoverflow.com/questions/39816779/check-if-type-is-defined#comment133524396_74039411
		/*
		int main()
		{
			class incomplete;
			class complete {};

			static_assert(!type_defined<incomplete>());
			static_assert(type_defined<complete>());
		}
		*/

	//----------------------------------------------------------------------------
	/*!!
		template<bool...> struct bool_pack;
		template<bool... bs> 
		using check_each = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;
		// Usage example: static_assert(check_each<( std::is_same< T, Vector_Interface<Vector2_Impl<NumT>> >() )...>::value);
		// https://stackoverflow.com/a/36934374/1479945
	!!*/

} // namespace internal //!! RENAME!


#endif // _DIOUNYWDIURBT570398FNY387NF8CU6DF344_
