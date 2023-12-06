#ifndef _SFW_COLOR_HPP_
#define _SFW_COLOR_HPP_

#include <cstdint>
#include <string>
#include <cassert>

//!! Auto-dispatch for the current backend instead (-> Gfx/backend.hpp):
//!! Also, don't just move this to Gfx/Color -- at lesat definitely not
//!! directly into that class (which shouldn't have all these deps)!...
#include <SFML/Graphics/Color.hpp>

namespace sfw {

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
}

/*!! Generalize, make it versatile etc.!

struct Color //!!?? or: RGBAColor : Color
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
};
!!*/

} // namespace

#endif // _SFW_COLOR_HPP_
