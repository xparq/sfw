//!!
//!! Don't just move this to a gfx/Color class -- at least definitely not
//!! directly into that class (which shouldn't have all these deps)!...
//!!

#ifndef _DFGM9W3804QWM89E70DFRNYGUYMCHW9D85MYHUV_
#define _DFGM9W3804QWM89E70DFRNYGUYMCHW9D85MYHUV_


#include "SAL.hpp"
#include SAL_ADAPTER(gfx/Color)

#include "SAL/util/diagnostics.hpp"

#include <cstdint> // uint32_t, uint8_t
#include <cstdlib> // strtoul
#include <string_view>
#include <string> // For a temorary inside the hex -> color converter... :-/


namespace SAL::gfx
{
	template <class Impl>
	class RGBAColor : public Impl
	{
	protected:
		constexpr       auto adapter()       { return static_cast<      Impl*>(this); }
		constexpr const auto adapter() const { return static_cast<const Impl*>(this); }

	public:
		enum : std::uint8_t {
			Transparent = 0,
			Translucent = 127,
			Opaque = 255,
		};

		using Impl::Impl;

		constexpr std::uint8_t& r()       { return adapter()->_r(); }
		constexpr std::uint8_t& g()       { return adapter()->_g(); }
		constexpr std::uint8_t& b()       { return adapter()->_b(); }
		constexpr std::uint8_t& a()       { return adapter()->_a(); }

		constexpr std::uint8_t  r() const { return adapter()->_r(); }
		constexpr std::uint8_t  g() const { return adapter()->_g(); }
		constexpr std::uint8_t  b() const { return adapter()->_b(); }
		constexpr std::uint8_t  a() const { return adapter()->_a(); }

		constexpr void          r(std::uint8_t red)   { adapter()->_r(red);   }
		constexpr void          g(std::uint8_t green) { adapter()->_g(green); }
		constexpr void          b(std::uint8_t blue)  { adapter()->_b(blue);  }
		constexpr void          a(std::uint8_t alpha) { adapter()->_a(alpha); }

		constexpr RGBAColor() = default; // uninitialized

		constexpr RGBAColor(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = Opaque)
		:	Impl(r, g, b, a) {}

		constexpr RGBAColor(std::uint32_t rgba)
		:	Impl(rgba) {}
		/*
		:	r(uint8_t((rgba & 0xff000000) >> 24)),
			g(uint8_t((rgba & 0x00ff0000) >> 16)),
			b(uint8_t((rgba & 0x0000ff00) >> 8)),
			a(uint8_t( rgba & 0x000000ff)) {}
		*/
		constexpr RGBAColor(std::uint32_t rgb, std::uint8_t alpha)
		:	Impl(rgb << 8 | alpha) {}
		/*
		:	r(uint8_t((rgb & 0xff000000) >> 16)),
			g(uint8_t((rgb & 0x00ff0000) >> 8)),
			b(uint8_t( rgb & 0x0000ff00)),
			a(alpha) {}
		*/
/*!!		
		explicit constexpr RGBAColor(std::string_view csshexcolor)
		{
			auto len = csshexcolor.size();

			SAL_ASSERT(len == 7 || len == 9 || len == 4 || len == 5);
			SAL_ASSERT(csshexcolor[0] == '#');

			// Note: strtoul will just return 0 for garbage.

			if (len >= 7) // #rrggbb[aa]
			{
				r( (uint8_t)strtoul(csshexcolor.substr(1, 2).c_str(), nullptr, 16) );
				g( (uint8_t)strtoul(csshexcolor.substr(3, 2).c_str(), nullptr, 16) );
				b( (uint8_t)strtoul(csshexcolor.substr(5, 2).c_str(), nullptr, 16) );
				a( (len == 9) ? (uint8_t)strtoul(csshexcolor.substr(7, 2).c_str(), nullptr, 16) : Opaque );
			}
			else if (len >= 4) // #rgb[a]
			{
				r( (uint8_t)strtoul(csshexcolor.substr(1, 1).c_str(), nullptr, 16) * 17 );
				g( (uint8_t)strtoul(csshexcolor.substr(2, 1).c_str(), nullptr, 16) * 17 );
				b( (uint8_t)strtoul(csshexcolor.substr(3, 1).c_str(), nullptr, 16) * 17 );
				a( (len == 5) ? (uint8_t)strtoul(csshexcolor.substr(4, 1).c_str(), nullptr, 16) * 17 : Opaque );
			}
			else
			{
				// Give a chance to catch it in DEBUG mode (but leave
				// it uninitialized, as in the default ctor):
				SAL_ASSERT(0); // Malformed color specifier!
			}
		}
!!*/

		explicit constexpr RGBAColor(std::string_view hexcolor_strview)
		{
			//!! strtoul() can't deal with string_views! :-/
			auto hexcolor = std::string(hexcolor_strview);

			SAL_ASSERT(hexcolor.size() == 7 || hexcolor.size() == 4 || hexcolor.size() == 9 || hexcolor.size() == 5);
			SAL_ASSERT(hexcolor[0] == '#');

#	define CCOMP(str, pos, width) (std::uint8_t)std::strtoul(hexcolor.substr(1 + (pos)*(width), width).c_str(), nullptr, 16)

			//!! No checking though!... But strtoul might even throw!... :-o
			switch (hexcolor.size())
			{
			case 7: // #rrggbb
				r( CCOMP(hexcolor, 0, 2) );
				g( CCOMP(hexcolor, 1, 2) );
				b( CCOMP(hexcolor, 2, 2) );
				a( 255 );
				break;
			case 4: // #rgb
				r( CCOMP(hexcolor, 0, 1) * 17 );
				g( CCOMP(hexcolor, 1, 1) * 17 );
				b( CCOMP(hexcolor, 2, 1) * 17 );
				a( 255 );
				break;
			case 9: // #rrggbbaa
				r( CCOMP(hexcolor, 0, 2) );
				g( CCOMP(hexcolor, 1, 2) );
				b( CCOMP(hexcolor, 2, 2) );
				a( CCOMP(hexcolor, 3, 2) );
				break;
			case 5: // #rgba
				r( CCOMP(hexcolor, 0, 1) * 17 );
				g( CCOMP(hexcolor, 1, 1) * 17 );
				b( CCOMP(hexcolor, 2, 1) * 17 );
				a( CCOMP(hexcolor, 3, 1) * 17 );
				break;
			default:;
			}
#	undef CCOMP

		}

	//!!	std::string get_hex() const (uint32_t rgb, uint8_t alpha = 255) { ... }

		// Generic (implicit) "get RGBA":
		constexpr operator std::uint32_t() const { return adapter()->_get_rgba(); } //!! OLD: return getRGBA();

		// Generic (implicit) "set RGBA":
		constexpr RGBAColor& operator = (std::uint32_t c) { rgba(c); return *this; }

		// The default op=(RGBAColor) is fine!

		constexpr std::uint32_t rgba() const { return adapter()->_get_rgba(); }
		constexpr std::uint32_t rgb()  const { return r() << 16 | g() << 8 | b(); }

		constexpr void rgba(std::uint32_t c_rgba) { adapter()->_set_rgba(c_rgba); }

		// Leave the alpha unmodified:
		constexpr void rgb(std::uint32_t c_rgb)                      { rgba(c_rgb << 8 | a()); }
		constexpr void rgb(uint8_t red, uint8_t green, uint8_t blue) { rgba((red << 24) | (green << 16) | (blue << 8) | a()); }

		// Set both the colors & alpha:
		constexpr void set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = Opaque) { rgba((red << 24) | (green << 16) | (blue << 8) | alpha); }
		constexpr void set(uint32_t c_rgb, uint8_t alpha = Opaque)                           { rgba((c_rgb << 8) | alpha); }


		constexpr bool operator == (std::uint32_t c) const { return rgba() == c; }
			// Other param. types are accepted via implicit conv. to uint32!


		// Some predefined colors, for convenience:
		static constinit const RGBAColor None; // Tansparent!
		static constinit const RGBAColor Black;
		static constinit const RGBAColor White;
		static constinit const RGBAColor Green;
		static constinit const RGBAColor Red;
		static constinit const RGBAColor Blue;
		static constinit const RGBAColor Yellow;
		static constinit const RGBAColor Magenta;
		static constinit const RGBAColor Cyan;

	}; // class RGBAColor<Impl>

///*!! I don't even know how to set these with a template, but
//     I do know it will be a fucking nightmare, yet again:
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::None    = 0x00000000;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Black   = 0x000000ff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::White   = 0xffffffff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Green   = 0x00ff00ff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Red     = 0xff0000ff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Blue    = 0x0000ffff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Yellow  = 0xffff00ff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Magenta = 0xff00ffff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Cyan    = 0x00ffffff;
//!!*/

/*!!
	class Color //!!?? or: RGBAColor : Color
	{
		enum : unsigned {
			Transparent = 0,
			Opaque = 255,
		};

		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color() = default; // uninitialized

		constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = Opaque)
		:	r(r), g(g), b(b), a(a) {}

		constexpr Color(uint32_t rgba)
		:	r(uint8_t((rgba & 0xff000000) >> 24)),
			g(uint8_t((rgba & 0x00ff0000) >> 16)),
			b(uint8_t((rgba & 0x0000ff00) >> 8)),
			a(uint8_t((rgba & 0x000000ff)) {}

		constexpr Color(uint32_t rgb, uint_8 alpha)
		:	r(uint8_t((rgb & 0xff000000) >> 16)),
			g(uint8_t((rgb & 0x00ff0000) >> 8)),
			b(uint8_t( rgb & 0x0000ff00),
			a(alpha) {}

		explicit constexpr Color(const std::string& csshexcolor)
		{
			auto len = hexcolor.size();

			assert(len == 7 || len == 9 || len == 4 || len == 5);
			assert(csshexcolor[0] == '#');

			// Note: strtoul will just return 0 for garbage.

			if (len >= 7) // #rrggbb[aa]
			{
				color.r = (uint8_t)strtoul(csshexcolor.substr(1, 2).c_str(), nullptr, 16);
				color.g = (uint8_t)strtoul(csshexcolor.substr(3, 2).c_str(), nullptr, 16);
				color.b = (uint8_t)strtoul(csshexcolor.substr(5, 2).c_str(), nullptr, 16);
				color.a = (len == 9) ? (uint8_t)strtoul(csshexcolor.substr(7, 2).c_str(), nullptr, 16) : Opaque;
			{
			else if (len >= 4) // #rgb[a]
			{
				color.r = (uint8_t)strtoul(csshexcolor.substr(1, 1).c_str(), nullptr, 16) * 17;
				color.g = (uint8_t)strtoul(csshexcolor.substr(2, 1).c_str(), nullptr, 16) * 17;
				color.b = (uint8_t)strtoul(csshexcolor.substr(3, 1).c_str(), nullptr, 16) * 17;
				color.a = (len == 5) ? (uint8_t)strtoul(csshexcolor.substr(4, 1).c_str(), nullptr, 16) * 17 : Opaque;
			}
			else
			{
				// Give a chance to catch it in DEBUG mode (but leave
				// it uninitialized, as in the default ctor):
				assert(csshexcolor == "well-formed");
			}
		}

		operator uint32_t() const { return getRGBA(); }

		void set(uint32_t rgb, uint8_t alpha) { setRGBA((rgb << 8) | alpha); }
	//!!	void setRGBA(uint32_t rgba), uint8_t alpha = 255) { ... }
	//!!	// Leave alpha unmodified:
	//!!	void setRGB( uint32_t rgb) uint8_t alpha = 255) { ... }

		uint32_t getRGBA() const { return (getRGB() << 8) | a; }
		uint32_t getRGB()  const { return (r << 24) | (g << 16) | (b << 8) | a; }

	//!!	std::string getCSShex() const (uint32_t rgb, uint8_t alpha = 255) { return (rgb << 8) | alpha; }

	//!!Move to some derived Color_SFML class:
	//!!	oprator sf::Color() { return sf::Color(getRGBA()); }
	}; // class Color
!!*/

/*!!
inline sf::Color Color(const std::string& hexcolor)
{
	assert(hexcolor.size() == 7 || hexcolor.size() == 4 || hexcolor.size() == 9 || hexcolor.size() == 5);
	assert(hexcolor[0] == '#');

	sf::Color color = sf::Color::Black;

#define CCOMP(str, pos, width) (uint8_t)std::strtoul(hexcolor.substr(1 + (pos)*(width), width).c_str(), nullptr, 16)
	switch (hexcolor.size())
	{
	case 7: // #rrggbb
		color.r = CCOMP(hexcolor, 0, 2);
		color.g = CCOMP(hexcolor, 1, 2);
		color.b = CCOMP(hexcolor, 2, 2);
		color.a = 255;
		break;
	case 4: // #rgb
		color.r = CCOMP(hexcolor, 0, 1) * 17;
		color.g = CCOMP(hexcolor, 1, 1) * 17;
		color.b = CCOMP(hexcolor, 2, 1) * 17;
		color.a = 255;
		break;
	case 9: // #rrggbbaa
		color.r = CCOMP(hexcolor, 0, 2);
		color.g = CCOMP(hexcolor, 1, 2);
		color.b = CCOMP(hexcolor, 2, 2);
		color.a = CCOMP(hexcolor, 3, 2);
		break;
	case 5: // #rgba
		color.r = CCOMP(hexcolor, 0, 1) * 17;
		color.g = CCOMP(hexcolor, 1, 1) * 17;
		color.b = CCOMP(hexcolor, 2, 1) * 17;
		color.a = CCOMP(hexcolor, 3, 1) * 17;
		break;
	default:;
	}
#undef CCOMP
	return color;

} // Color()
!!*/


	//--------------------------------------------------------------------
	// The actual API ("dictionary") type:
	using Color = RGBAColor<adapter::RGBAColor_Impl>;


	////////////////////////////////////////////////////////////
	//!! Componentwise ops. ripped from SFML!...
	////////////////////////////////////////////////////////////

	constexpr Color operator+(Color left, Color right)
	{
		const auto clampedAdd = [](std::uint8_t lhs, std::uint8_t rhs) -> std::uint8_t
		{
			int result = int(lhs) + int(rhs);
			return std::uint8_t(result < 255 ? result : 255);
		};

		return {clampedAdd(left.r(), right.r()),
		        clampedAdd(left.b(), right.b()),
		        clampedAdd(left.g(), right.g()),
		        clampedAdd(left.a(), right.a())};
	}

	constexpr Color operator-(Color left, Color right)
	{
		const auto clampedSub = [](std::uint8_t lhs, std::uint8_t rhs) -> std::uint8_t
		{
			int result = int(lhs) - int(rhs);
			return std::uint8_t(result > 0 ? result : 0);
		};

		return {clampedSub(left.r(), right.r()),
		        clampedSub(left.g(), right.g()),
		        clampedSub(left.b(), right.b()),
		        clampedSub(left.a(), right.a())};
	}

	constexpr Color operator*(Color left, Color right)
	{
		const auto scaledMul = [](std::uint8_t lhs, std::uint8_t rhs) -> std::uint8_t
		{
			auto result_u16 = std::uint16_t(std::uint16_t(lhs) * std::uint16_t(rhs));
			return std::uint8_t(result_u16 / 255u);
		};

		return {scaledMul(left.r(), right.r()),
		        scaledMul(left.g(), right.g()),
		        scaledMul(left.b(), right.b()),
		        scaledMul(left.a(), right.a())};
	}

	////////////////////////////////////////////////////////////

	constexpr Color& operator+=(Color& left, Color right) { return left = left + right; }
	constexpr Color& operator-=(Color& left, Color right) { return left = left - right; }
	constexpr Color& operator*=(Color& left, Color right) { return left = left * right; }


} // namespace SAL::gfx


#endif // _DFGM9W3804QWM89E70DFRNYGUYMCHW9D85MYHUV_
