#ifndef _ASD8745YUYNGM8304C8M5NYPI8UYH398V5BMNYHU_
#define _ASD8745YUYNGM8304C8M5NYPI8UYH398V5BMNYHU_


#include "SAL.hpp"
#include SAL_ADAPTER(event/Input)


namespace SAL::event
{
	//!! LEGACY LEFTOVER EXPERIM. codes for widget updates (or whatnot):
	enum ID
	{
		Noop,
		Update,
		Change,
		Commit,

		_EVENTS_
	};


	//!!...
	using Input = adapter::Input_Impl;

} // namespace SAL::event


#endif // _ASD8745YUYNGM8304C8M5NYPI8UYH398V5BMNYHU_
