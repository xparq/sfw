#ifndef _K1MF928TY67I4D25978Y7H97RHY98UERTF356UT6_
#define _K1MF928TY67I4D25978Y7H97RHY98UERTF356UT6_


#include "SAL.hpp"
#include SAL_ADAPTER(event/Keyboard)


#include <cstdint> // uint32_t


namespace SAL::event
{
	//------------------------------------------------
	using KeyState = /*!!adapter::!!*/KeyState;

	//!!DEPRECATED:
	using KeyCombination = KeyState;


	//------------------------------------------------
	struct TextInput {
		std::uint32_t codepoint;
	};
}


#endif // _K1MF928TY67I4D25978Y7H97RHY98UERTF356UT6_
