#ifndef _FLSMYIKUEWRYRTF87N34Y87EY5RI7HUYJFG_
#define _FLSMYIKUEWRYRTF87N34Y87EY5RI7HUYJFG_


#include "sfw/geometry/Rectangle.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>

#include <string_view>


namespace sfw
{

class Texture : public sf::Texture
{
public:
	using native_type = sf::Texture;

	using sf::Texture::Texture; // Reuse the SFML ctors.

	Texture() : sf::Texture(sf::Texture::create({1, 1}).value()) {} //!! Assumed never to fail...

	bool load(std::string_view filename, const geometry::iRect& r = {})
		{ auto res = sf::Texture::loadFromFile(filename, false, r); // false: no sRTGB support in this adapter...
		  if (res) { native() = res.value(); } return res.has_value(); }

	bool set(const sf::Image& image, const geometry::iRect& r = {})
		{ auto res = sf::Texture::loadFromImage(image, false, r); // false: no sRTGB support in this adapter...
		  if (res) { native() = res.value(); } return res.has_value(); }

	iVec2 size() const { return native().getSize(); }

	//!! Since we're just subclassing sf::Texture, these are redundant:
	constexpr const native_type& native() const { return *this; }
	constexpr       native_type& native()       { return *this; }

//!! Since we're just subclassing sf::Texture, these are redundant:
//!!	constexpr operator const native_type() const { return native(); }
//!!	constexpr operator       native_type&()      { return native(); }

}; // class Texture

} // namespace sfw


#endif // _FLSMYIKUEWRYRTF87N34Y87EY5RI7HUYJFG_
