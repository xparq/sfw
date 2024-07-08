#ifndef _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_
#define _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_


//!! Include the Vector concept(s) instead (which should grab Scalar, too)!
#include "SAL/math/Scalar.hpp"

#include <SFML/System/Vector2.hpp>

#include <utility> // forward


namespace SAL//!!::math
{
namespace adapter::SFML
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

		template <Vector V>
		constexpr Vector2_Impl(V&& v) : _d(v.x(), v.y()) {}
//!!			{ static_assert(std::is_same_v<IVector::number_type, NumT>); }
//!!??			{ static_assert(std::is_convertible_v<IVector::number_type, NumT>); }

		//!! No need for these with the template below:
		//constexpr Vector2_Impl() : _d() {}
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

	template <Scalar NumT>
	Vector2_Impl(NumT x, NumT y) -> Vector2_Impl<NumT>;
} // namespace adapter::SFML


// Save the specific adapter type to a backend-agnostic symbol...
namespace adapter//::active_backend
{
	template <Scalar NumT, unsigned Dim = 2>
	using Vector2_Impl = SFML::Vector2_Impl<NumT, Dim>; //!! Use a "standard" SAL macro for the indirectly name the current backend!
}


} // namespace SAL//!!::math


#endif // _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_
