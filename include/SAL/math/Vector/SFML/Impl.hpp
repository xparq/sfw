﻿#ifndef _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_
#define _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_


//!! Include the Vector concept(s) instead (which should grab Scalar, too)!
#include "SAL/math/Scalar.hpp"

#include <SFML/System/Vector2.hpp>

#include <utility> // forward
#include <type_traits> // is_same, enable_if


namespace SAL/*!!::math!!*/::adapter
{
namespace SFML
{
	template <Scalar NumT, unsigned Dim = 2> //!! Dim is totally unused; just a reminder!
	class Vector2_Impl
	{
	public:
		constexpr static inline auto dimensions = Dim; //!!??C++: Is "cx. st. inline" the correct incantation?
		using number_type = NumT;
		using native_type = sf::Vector2<NumT>;

	private:
		native_type _d;

	public:
		constexpr       auto& native()       { return _d; }
		constexpr const auto& native() const { return _d; }

		// Null vector, with explicit zero-init:
		constexpr Vector2_Impl() : _d(0, 0) {}

		//!! Making this a teamplate only because MSVC seems to still go for implicit conversions without it:
		//!! - Except it didn't help!... :-o
		template <Scalar T> //!! This didn't fix the weird MSVC leniency either: ..., typename = std::enable_if_t<std::is_same_v<T, NumT>> >
			requires (std::is_same_v<T, NumT>)
		constexpr Vector2_Impl(number_type x, number_type y) : _d(x, y) {}

		// Convenience conversions from common numeric types, when different from NumT:
		//!! These might hide actual accidents, though!...
		//!!NOTE: With these in the Interface class (as ... : Impl(...) {} ) each basic init type deduction test case failed! :-o
		template <Scalar T>
			requires (!std::is_same_v<T, NumT>)
		constexpr explicit Vector2_Impl(T x, T y) : _d((number_type)x, (number_type)y) {}

/*!!?? WHAT IF THIS ISN'T HERE?...
		template <Vector V>
		constexpr Vector2_Impl(V&& v) : _d((number_type)v.x(), (number_type)v.y()) {}
//!!			{ static_assert(std::is_same_v<IVector::number_type, NumT>); }
//!!??			{ static_assert(std::is_convertible_v<IVector::number_type, NumT>); }
??!!*/
		//!! No need for these with the template below:
		//constexpr Vector2_Impl(sf::Vector2<NumT> v) : _d(v) {}
		//!! Alas, this alone still won't make Vect2 v{x, y} work, though (no ADL
		//!! from ctor params: https://stackoverflow.com/a/29677814/1479945):
		template <typename... T>
		constexpr Vector2_Impl(T&&... args) : _d(std::forward<T>(args)...) {}
		//!! Adding this explicitly (and moving above the template) doesn't help:
		//!!constexpr Vector2_Impl(NumT x, NumT y) : _d(x, y) {}


		constexpr auto& _x()       { return _d.x; }
		constexpr auto& _y()       { return _d.y; }
		constexpr auto  _x() const { return _d.x; }
		constexpr auto  _y() const { return _d.y; }

		// Since op<=> will be deleted by the Ifc class, it can't be relied on...
		bool operator == (const Vector2_Impl&) const = default;
	};

/*!!?? This prevents accepting other types than NumT, though (for explicit conversion by the ctor)!
	template <Scalar NumT>
	Vector2_Impl(NumT x, NumT y) -> Vector2_Impl<NumT>;
??!!*/
} // namespace SFML


	// Save the specific adapter type to a backend-agnostic symbol...
	//!!
	//!! This looks totally useless, as this entire header is dispached by the selected backend,
	//!! so things defined here could just live directly in adapter::, no need for SFML:: too
	//!! -- but... The build system can't also dispatch the translation units yet, so it would
	//!! just compile -- and link -- everything together, causing name clashes across backends!
	//!!
	template <Scalar NumT, unsigned Dim = 2>
	using Vector2_Impl = SFML::Vector2_Impl<NumT, Dim>; //!! Use a "standard" SAL macro for the indirectly name the current backend!


} // namespace SAL/*!!::math!!*/::adapter


#endif // _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_
