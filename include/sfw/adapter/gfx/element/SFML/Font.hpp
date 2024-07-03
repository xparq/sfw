#ifndef _FC24542C245V0N689YWDRTUGNH87C4VGDR8Y7V346_
#define _FC24542C245V0N689YWDRTUGNH87C4VGDR8Y7V346_


#include <SFML/Graphics/Font.hpp>

#include <memory> // unique_ptr
#include <string_view>
#include <type_traits> // is_default_constructible etc.
#include <utility> // ignore
#include <optional>

#include "sfw/util/cpp.hpp" // SFW_CONST_THIS


namespace sfw
{

static_assert( ! std::is_default_constructible_v<sf::Font> );
// But it's copyable?! :-o WHY?! It's NOT just a handle!
//!!?! And how?! It doesn't have a copy ctor, only a user-defined custom one! :-o
static_assert( std::is_copy_constructible_v<sf::Font> );


class Font_Impl //!! Can't just derive from sf::Font, as it's not default-constructible...
{
public:
	using native_type = sf::Font;

/*	bool load(std::string_view filename) {
		auto result = sf::Font::openFromFile(filename);
		if (result) native(result.value()); // Save a pointer to the original!
		return result.has_value();
	}
*/
	bool load(std::string_view filename) {
		_native_font = sf::Font::openFromFile(filename);
		return _valid();
	}

	//--------------------------------------------------------------------
	// Queries...

	constexpr bool _valid() const { return (bool)_native_font; }

	constexpr float _get_line_spacing(unsigned fontSize) const { return native().getLineSpacing(fontSize); }


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


	constexpr const native_type& native() const {
		_err_if_null();
//!!		return *_native_font_ptr;
		return _native_font.value();
	}

/*!!
	//!! Can't just also allow assigning: must free and/or allocate etc., so needs a real setter! :-/
	constexpr const native_type& native(const native_type& native_font) {
		if (_native_font_ptr) delete _native_font_ptr;
		_native_font_ptr = new native_type(native_font);
		return SFW_CONST_THIS()->native();
	}
!!*/
/*
	// Basically op=...
	constexpr void native(native_type&& native_font) {
		_native_font.swap(native_type(native_font));
		return SFW_CONST_THIS()->native();
	}
*/

//!!??WTF, WOULD THIS ALSO DELETE THE CONST VERSION?!
//!!	constexpr native_type&       native() = delete;

	//--------------------------------------------------------------------
	// Internals...
	//
protected:

	//!! CONSOLIDATE:
	void _err_if_null() const { if (!_valid()) throw ("- no font!"); }


	//--------------------------------------------------------------------
	// Data...
	//
private:
	//!! native_type* _native_font_ptr = nullptr;
	//!!unique_ptr<native_type> _native_font_ptr;
	std::optional<sf::Font> _native_font;

}; // class Font_Impl

} // namespace sfw


#endif // _FC24542C245V0N689YWDRTUGNH87C4VGDR8Y7V346_
