#ifndef _FDPOTUFY48705WUHY97G85UM789E5N6098N7M59G8_
#define _FDPOTUFY48705WUHY97G85UM789E5N6098N7M59G8_


//----------------------------------------------------------------------------
// 2c99b33@SFML removed the default sf::Text ctor with no font.
// This helper class can be used to help existing APIs continue to rely
// on the default font implicitly.

#include "sfw/Theme.hpp" //!!?? For the "current font"; But could it not depend on the Theme actually?

#include "sfw/adapter/dispatch.hpp"
#include SFW_ADAPTER_IMPL(gfx/element, Font.hpp)

#include <string_view>


namespace sfw
{

// Basically just a pointer class, as the original font resource is managed by the backend.
class Font : public Font_Impl
{
	using Impl = Font_Impl;

public:
	constexpr Font() = default;
	constexpr Font(const Font&) = delete; // No copy!
	Font(std::string_view filename) { load(filename); }

	constexpr operator bool () const { return _valid(); }

	//--------------------------------------------------------------------
	// Queries...

	float lineSpacing(unsigned fontSize) const { return Impl::_get_line_spacing(fontSize); }

}; // class Font

} // namespace sfw


#endif // _FDPOTUFY48705WUHY97G85UM789E5N6098N7M59G8_
