#ifndef _DM08T5DNEHTUDIGYHNXDM8THD87GHERNTGYF7F_
#define _DM08T5DNEHTUDIGYHNXDM8THD87GHERNTGYF7F_

//----------------------------------------------------------------------------
// Dispatch backend-specific #includes
//
//https://stackoverflow.com/a/50728314/1479945 <- Reminder that `#include MACRO` is a thing...

#ifndef SFW_GFX_BACKEND
//!!Should only be a warning about including some default!
//!!But #warning is c++23(!...), and isn't supported by MSVC yet. :-/
//!!(Note that GCC has supported it for ages.)
//!!#warning No backend has been selected. Use: #include "sfw/cfg/USE_{backendname}"
//!!#warning Selecting SFML as default graphics backend...
#include "sfw/cfg/USE_SFML"
#endif

//!! Get these from a common language shim header instead:
#define _sfw_STR_IMPL(X) #X
#define _sfw_STR(X) _sfw_STR_IMPL(X)

#define GFX_BACKEND_SPECIFIC(filename) _sfw_STR(sfw/gfx/_backend-specific/SFW_GFX_BACKEND/filename)
// Usage: #include GFX_BACKEND_SPECIFIC(headername.h)

#endif // _DM08T5DNEHTUDIGYHNXDM8THD87GHERNTGYF7F_
