#ifndef _FDPOTUFY48705WUHY97G85UM789E5N6098N7M59G8_
#define _FDPOTUFY48705WUHY97G85UM789E5N6098N7M59G8_


#include "SAL.hpp"
#include SAL_ADAPTER(gfx/element/Font)

#include <string_view>


namespace SAL::gfx
{

// Basically just a pointer class, as the original font resource is managed by the backend.
class Font : public adapter::Font_Impl
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


} // namespace SAL::gfx


#endif // _FDPOTUFY48705WUHY97G85UM789E5N6098N7M59G8_
