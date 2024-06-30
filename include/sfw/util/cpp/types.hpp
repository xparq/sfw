#ifndef _DSSKJHDSJKGHSDJKLGHDSLJKGH8943HT2745H20724H50824HT084_
#define _DSSKJHDSJKGHSDJKLGHDSLJKGH8943HT2745H20724H50824HT084_

//!! Move these to an even more generic lang. layer (e.g. sz.lib)!


#include <type_traits>
#include <concepts>

namespace sfw //!! sz
{

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


//----------------------------------------------------------------------------
//!!C++: It's a shame not having sg. like std::number already. (https://stackoverflow.com/a/58067611/1479945)
template<typename T> concept Scalar = std::integral<T> || std::floating_point<T>;

} // namespace sfw //!! sz


#endif // _DSSKJHDSJKGHSDJKLGHDSLJKGH8943HT2745H20724H50824HT084_
