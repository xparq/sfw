#ifndef _CDFVOMEBINRD87FYMH39THMY89546YM856H89I6RH_
#define _CDFVOMEBINRD87FYMH39THMY89546YM856H89I6RH_


#include <cstdint> // uint32_t, uint8_t
#include <utility> // forward

//#include "SAL/util/diagnostics.hpp"

#include <SFML/Graphics/Color.hpp>


namespace SAL::gfx::adapter
{
	class RGBAColor_Impl
	{
	protected:
		using native_type = sf::Color;
	
		native_type _d;

		constexpr const native_type& native() const { return _d; }
		constexpr       native_type& native()       { return _d; }

	public:
//!! This all-in implicit forwarding leaks backend API dependencies through the interface layer!
//!!		// Forward to the backend ctor:
//!!		template <typename... T>
//!!		constexpr RGBAColor_Impl(T&&... args) : _d(std::forward<T>(args)...) {}

		constexpr RGBAColor_Impl() {} // No init!

		constexpr RGBAColor_Impl(std::uint32_t c)
			: _d(c) {}

		constexpr RGBAColor_Impl(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
			: _d(r, g, b, a) {}

		// Convert from the backend type:
		constexpr RGBAColor_Impl(native_type n) : _d(n) {}

		// Convert to the backend type:
		constexpr operator const native_type& () const { return native(); }
		constexpr operator       native_type& ()       { return native(); }

//		constexpr RGBAColor_Impl& operator = (const RGBAColor_Impl&) = default;
		constexpr RGBAColor_Impl& operator = (std::uint32_t rgba) { _set_rgba(rgba); return *this; }

		constexpr std::uint8_t& _r()       { return _d.r; }
		constexpr std::uint8_t& _g()       { return _d.g; }
		constexpr std::uint8_t& _b()       { return _d.b; }
		constexpr std::uint8_t& _a()       { return _d.a; }
		constexpr std::uint8_t  _r() const { return _d.r; }
		constexpr std::uint8_t  _g() const { return _d.g; }
		constexpr std::uint8_t  _b() const { return _d.b; }
		constexpr std::uint8_t  _a() const { return _d.a; }

		constexpr void          _r(std::uint8_t red)   { _d.r = red;   }
		constexpr void          _g(std::uint8_t green) { _d.g = green; }
		constexpr void          _b(std::uint8_t blue)  { _d.b = blue;  }
		constexpr void          _a(std::uint8_t alpha) { _d.a = alpha; }

		constexpr void          _set_rgba(std::uint32_t rgba) { _d = native_type(rgba); }
		constexpr std::uint32_t _get_rgba() const { return _d.toInteger(); }

	}; // class RGBAColor_Impl

} // namespace SAL::gfx::adapter


#endif // _CDFVOMEBINRD87FYMH39THMY89546YM856H89I6RH_
