#ifndef _DFGM9W3804QWM89E70DFRNYGUYMCHW9D85MYHUV_
#define _DFGM9W3804QWM89E70DFRNYGUYMCHW9D85MYHUV_


#include "SAL.hpp"
#include SAL_ADAPTER(gfx/Color)

#include <cstdint>  // uint32_t, uint8_t

//!! Move the from-text conv. from this class (which shouldn't have these deps)
//!! to an optionally included component!...
#include <string_view>
#include <charconv> // from_chars

#include "SAL/util/diagnostics.hpp"


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

		constexpr RGBAColor(std::uint32_t rgb, std::uint8_t alpha)
		:	Impl(rgb << 8 | alpha) {}

		explicit constexpr RGBAColor(std::string_view csshexcolor)
		{
			auto len = csshexcolor.length();

			//!! No runtime format checking! But, well, the worst case is a bogus color...
			SAL_ASSERT(len == 7 || len == 9 || len == 4 || len == 5);
			SAL_ASSERT(csshexcolor[0] == '#');

			constexpr auto CComp = [](const std::string_view& str, unsigned pos, unsigned width) {
				SAL_ASSERT(                  1 + width*pos + width <= str.length());
				const char* p = str.data() + 1 + width*pos;
				uint8_t n = 0;
				//auto [p, err] = 
					std::from_chars(p, p + width, n, 16);
				return n;
			};

			switch (len)
			{
			case 7: // #rrggbb
				r( CComp(csshexcolor, 0, 2) );
				g( CComp(csshexcolor, 1, 2) );
				b( CComp(csshexcolor, 2, 2) );
				a( Opaque );
				break;
			case 4: // #rgb
				r( CComp(csshexcolor, 0, 1) * 17 );
				g( CComp(csshexcolor, 1, 1) * 17 );
				b( CComp(csshexcolor, 2, 1) * 17 );
				a( Opaque );
				break;
			case 9: // #rrggbbaa
				r( CComp(csshexcolor, 0, 2) );
				g( CComp(csshexcolor, 1, 2) );
				b( CComp(csshexcolor, 2, 2) );
				a( CComp(csshexcolor, 3, 2) );
				break;
			case 5: // #rgba
				r( CComp(csshexcolor, 0, 1) * 17 );
				g( CComp(csshexcolor, 1, 1) * 17 );
				b( CComp(csshexcolor, 2, 1) * 17 );
				a( CComp(csshexcolor, 3, 1) * 17 );
				break;
			default:;
			}
		}

/*!! Shorter, but more branching:

		explicit constexpr RGBAColor(std::string_view csshexcolor)
		{
			auto len = csshexcolor.length();

			//!! No runtime format checking! But, well, the worst case is a bogus color...
			SAL_ASSERT(len == 7 || len == 9 || len == 4 || len == 5);
			SAL_ASSERT(csshexcolor[0] == '#');

			constexpr auto CComp = [](const std::string_view& str, unsigned pos, unsigned width) {
				SAL_ASSERT(pos + width <= str.length());
				const char* p = str.data() + pos;
				uint8_t n = 0;
				//auto [p, err] = 
					std::from_chars(p, p + width, n, 16);
				return n;
			};

			if (len >= 7) // #rrggbb[aa]
			{
				r( CComp(csshexcolor, 1, 2) );
				g( CComp(csshexcolor, 3, 2) );
				b( CComp(csshexcolor, 5, 2) );
				a( (len == 9) ? CComp(csshexcolor, 7, 2) : (uint8_t)Opaque );
			}
			else if (len >= 4) // #rgb[a]
			{
				r( CComp(csshexcolor, 1, 1) * 17 );
				g( CComp(csshexcolor, 2, 1) * 17 );
				b( CComp(csshexcolor, 3, 1) * 17 );
				a( (len == 5) ? CComp(csshexcolor, 4, 1) * 17 : (uint8_t)Opaque );
			}
			else
			{
				// Give a chance to catch it in DEBUG mode (but leave
				// it uninitialized, as in the default ctor):
				SAL_ASSERT(0); // Malformed color specifier!
			}
		}
!!*/
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
		static constinit const RGBAColor None; // Tansparent black...
		static constinit const RGBAColor Black;
		static constinit const RGBAColor White;
		static constinit const RGBAColor Green;
		static constinit const RGBAColor Red;
		static constinit const RGBAColor Blue;
		static constinit const RGBAColor Yellow;
		static constinit const RGBAColor Magenta;
		static constinit const RGBAColor Cyan;

	}; // class RGBAColor<Impl>

		//!! I didn't even know how to write these properly if templated, but
		//!! I did know it was gonna be a fucking nightmare, again...
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::None    = 0x00000000;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Black   = 0x000000ff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::White   = 0xffffffff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Green   = 0x00ff00ff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Red     = 0xff0000ff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Blue    = 0x0000ffff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Yellow  = 0xffff00ff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Magenta = 0xff00ffff;
		template <class Impl> constinit const RGBAColor<Impl> RGBAColor<Impl>::Cyan    = 0x00ffffff;


/*!! Don't forget that this can also be a free function (but not with this obsolete impl.):
inline sf::Color Color(const std::string& csshexcolor)
{
	assert(csshexcolor.length() == 7 || csshexcolor.length() == 4 || csshexcolor.length() == 9 || csshexcolor.length() == 5);
	assert(csshexcolor[0] == '#');

	sf::Color color = sf::Color::Black;

#define CComp(str, pos, width) (uint8_t)std::strtoul(csshexcolor.substr(1 + (pos)*(width), width).c_str(), nullptr, 16)
	switch (csshexcolor.length())
	{
	case 7: // #rrggbb
		color.r = CComp(csshexcolor, 0, 2);
		color.g = CComp(csshexcolor, 1, 2);
		color.b = CComp(csshexcolor, 2, 2);
		color.a = 255;
		break;
	case 4: // #rgb
		color.r = CComp(csshexcolor, 0, 1) * 17;
		color.g = CComp(csshexcolor, 1, 1) * 17;
		color.b = CComp(csshexcolor, 2, 1) * 17;
		color.a = 255;
		break;
	case 9: // #rrggbbaa
		color.r = CComp(csshexcolor, 0, 2);
		color.g = CComp(csshexcolor, 1, 2);
		color.b = CComp(csshexcolor, 2, 2);
		color.a = CComp(csshexcolor, 3, 2);
		break;
	case 5: // #rgba
		color.r = CComp(csshexcolor, 0, 1) * 17;
		color.g = CComp(csshexcolor, 1, 1) * 17;
		color.b = CComp(csshexcolor, 2, 1) * 17;
		color.a = CComp(csshexcolor, 3, 1) * 17;
		break;
	default:;
	}
#undef CComp
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
