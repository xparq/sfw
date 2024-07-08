#ifndef _FWMW9S8TYHJMFC3569YF890UYM498G56UY894T_
#define _FWMW9S8TYHJMFC3569YF890UYM498G56UY894T_


//!! Include the Rectangle concept(s) instead, which should grab the Vector, too!
//!! (Ideally then, there should be no need to use the Vec aliases here at all.)
#include "SAL/math/Vector.hpp" //!! Vector Adapter Interface, actually... Only the concept header shouild be called Vector!

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp> // Rect has it; just making its use explicit here.

#include <utility> // forward


namespace SAL::adapter
{
namespace SFML
{
	template <typename NumT>
	class Rectangle_Impl
	{
	public:
		using number_type = NumT;
		using vector_type = Vec2<NumT>;
		using native_type = sf::Rect<NumT>;

	private:
		native_type _d;

	public:
		constexpr       auto& native()       { return _d; }
		constexpr const auto& native() const { return _d; }

		// Intercept our vectors:
		constexpr Rectangle_Impl(Vec2<NumT> pos, Vec2<NumT> size)               : _d(pos.native(), size.native()) {}
//!!?? OR:	constexpr Rectangle_Impl(const Vec2<NumT>& pos, const Vec2<NumT>& size) : _d(pos.native(), size.native()) {}

		// Forward the original signatures:
		template <typename... T>
		constexpr Rectangle_Impl(T&&... args) : _d(std::forward<T>(args)...) {}

		// Core get/set impl.:
		constexpr auto  _x()  const { return _d.position.x; }
		constexpr auto  _y()  const { return _d.position.y; }
		constexpr auto  _dx() const { return _d.size.x; }
		constexpr auto  _dy() const { return _d.size.y; }
		constexpr auto& _x()        { return _d.position.x; }
		constexpr auto& _y()        { return _d.position.y; }
		constexpr auto& _dx()       { return _d.size.x; }
		constexpr auto& _dy()       { return _d.size.y; }
	};

	template <Scalar NumT>
	Rectangle_Impl(Vec2<NumT> pos, Vec2<NumT> size) -> Rectangle_Impl<NumT>;

	template <Scalar NumT>
	Rectangle_Impl(sf::Vector2<NumT> pos, sf::Vector2<NumT> size) -> Rectangle_Impl<NumT>;
//!!??	Rectangle_Impl(Vec2<NumT>::native_type pos, Vec2<NumT>::native_type size) -> Rectangle_Impl<NumT>;

} // namespace SFML


// Save the specific adapter type to a backend-agnostic symbol...
//namespace active_backend
//{
	template <typename NumT>
	using Rectangle_Impl = SFML::Rectangle_Impl<NumT>;
//}

} // namespace SAL::adapter


#endif // _FWMW9S8TYHJMFC3569YF890UYM498G56UY894T_
