//----------------------------------------------------------------------------
// 2c99b33@SFML removed the default sf::Text ctor with no font.
// This helper class can be used to help existing APIs continue to rely
// on the default font implicitly.
//----------------------------------------------------------------------------

#ifndef _FC24542C245V0N689YWDRTUGNH87C4VGDR8Y7V346_
#define _FC24542C245V0N689YWDRTUGNH87C4VGDR8Y7V346_


#include <SFML/Graphics/Font.hpp>

#include <memory> // unique_ptr
#include <string_view>
#include <type_traits> // is_default_constructible etc.
#include <utility> // ignore
#include <optional>

//#include "SAL/util/cpp/macros.hpp" // SAL_CONST_THIS

//!!#include <cassert>


namespace SAL::gfx::adapter
{

namespace SFML
{
//static_assert( ! std::is_default_constructible_v<sf::Font> );
// But it's copyable?! :-o WHY?! It's NOT just a handle!
//!!?! And how?! It doesn't have a copy ctor, only a user-defined custom one! :-o
static_assert( std::is_copy_constructible_v<sf::Font> );


class Font_Impl //!! Can't just derive from sf::Font, as it's not default-constructible...
{
public:
	using native_type = sf::Font;

	bool load(std::string_view filename) {
		_native_font = sf::Font::createFromFile(filename);
		return _valid();
	}

	//--------------------------------------------------------------------
	// Queries...

	constexpr bool _valid() const { return (bool)_native_font; } //!!?? Or has_value()?

	float _get_line_spacing(unsigned fontSize) const { return native().getLineSpacing(fontSize); }


	//--------------------------------------------------------------------
	// Construct/destruct...
	//

	//!! No SFML ctors any more:
	//!!using sf::Font::Font; // Reuse the SFML ctors.

	constexpr Font_Impl() = default; // {} // Allow null state... Null-ptr access (only possible via native()) will be checked.
	constexpr Font_Impl(const Font_Impl&) = delete; // No copy!
//	constexpr ~Font_Impl() = default; //{ if (_native_font_ptr) delete _native_font_ptr; }

	//--------------------------------------------------------------------
	// Auto-convert to the native type...
	//
	constexpr operator const native_type& () const { return native(); }
	//!! But not writable (see the `native(...)` setter for that!):
//!!??WTF, WOULD THIS ALSO DELETE THE CONST VERSION?!
//!!	constexpr operator native_type& () = delete;


/*!!
	//!! Can't just also allow assigning: must free and/or allocate etc., so needs a real setter! :-/
	constexpr const native_type& native(const native_type& native_font) {
		if (_native_font_ptr) delete _native_font_ptr;
		_native_font_ptr = new native_type(native_font);
		return SAL_CONST_THIS()->native();
	}
!!*/
/*
	// Basically op=...
	constexpr void native(native_type&& native_font) {
		_native_font.swap(native_type(native_font));
//!!		return SAL_CONST_THIS()->native();
	}
*/

//!!??WTF, WOULD THIS ALSO DELETE THE CONST VERSION?!
//!!	constexpr native_type&       native() = delete;

	//--------------------------------------------------------------------
	// Internals...
	//
protected:

	//!! CONSOLIDATE:
	//!! First off, it should theoretically be an assert, as always having a native font is an invariant!
	//!!?? But then why is it an std::optional at all? Is it ONLY because we get it that way from SFML,
	//!!?? and we just shouldn't (and can't?) copy it out from there?
	constexpr void _err_if_null() const { if (!_valid()) throw ("- No sf::Font in SAL::Font?!"); }


	//--------------------------------------------------------------------
	// Data...
	//

private:
	//!! native_type* _native_font_ptr = nullptr;
	//!!unique_ptr<native_type> _native_font_ptr;
	std::optional<sf::Font> _native_font;
		//!!?? Is it std::optional ONLY because we get it that way from SFML,
		//!!?? and we just shouldn't (and can't?) copy it out from there?
		//!! Note: std::optional can at least start empty, so we can set (and reset) it after constr.!

public: // Only public for diagnostics (hopefully...):
	constexpr const native_type& native() const {
		_err_if_null();
		return _native_font.value(); // Just a ref.
	}

	constexpr native_type& native() {
		_err_if_null();
		return _native_font.value(); // Just a ref.
	}

}; // class Font_Impl


} // namespace SFML


// Save this Impl type to a backend-independent symbol:
using Font_Impl = SFML::Font_Impl; //!! Use a "standard" SAL macro for the indirectly name the current backend!


} // namespace SAL::gfx::adapter


#endif // _FC24542C245V0N689YWDRTUGNH87C4VGDR8Y7V346_
