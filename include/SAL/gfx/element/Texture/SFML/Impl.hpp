#ifndef _FLSMYIKUEWRYRTF87N34Y87EY5RI7HUYJFG_
#define _FLSMYIKUEWRYRTF87N34Y87EY5RI7HUYJFG_

//----------------------------------------------------------------------------
// An SFML commit in 2024 removed the default sf::Texture ctor. :-(
// This helper class can be used to help existing APIs continue to rely
// on a non-RAII, non-dynamically created empty texture.

#include "SAL/geometry/Rectangle.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>

#include <string_view>


namespace SAL::gfx
{

class Texture : public sf::Texture
{
public:
	using native_type = sf::Texture;

	using sf::Texture::Texture; // Reuse the SFML ctors.

	Texture() : sf::Texture(sf::Texture::create({1, 1}).value()) {} //!! Assumed never to fail...

	bool load(std::string_view filename, const geometry::iRect& r = {})
		{ auto res = sf::Texture::createFromFile(filename, false, r); // false: no sRTGB support in this adapter...
		  if (res) { native() = res.value(); } return res.has_value(); }

	bool set(const sf::Image& image, const geometry::iRect& r = {})
		{ auto res = native().loadFromImage(image, false, r); // false: no sRTGB support in this adapter...
//!!OLD:	  if (res) { native() = res.value(); } return res.has_value(); }
	          return res; }

	iVec2 size() const { return iVec2(uVec2(native().getSize())); } // getSize() is unsigned!
 	//!! xparq/pocketvec#18:
	//!!OK:       iVec2 size() const { auto [x, y] = native().getSize(); return iVec2(x, y); }
	//!!ALSO OK!: iVec2 size() const { return iVec2(native().getSize()); } //!! This is an "explicit" SFML ctor call as far as C++ is concerned! :-o
	//!!BUT NOT THIS, unless VEC_IMPLICIT_NUM_CONV:
	//!!          iVec2 size() const { return uVec2(native().getSize()); }

	//!! Since we're just subclassing sf::Texture, these are redundant:
	constexpr const native_type& native() const { return *this; }
	constexpr       native_type& native()       { return *this; }

//!! Since we're just subclassing sf::Texture, these are redundant:
//!!	constexpr operator const native_type() const { return native(); }
//!!	constexpr operator       native_type&()      { return native(); }

}; // class Texture

} // namespace SAL::gfx


#endif // _FLSMYIKUEWRYRTF87N34Y87EY5RI7HUYJFG_
