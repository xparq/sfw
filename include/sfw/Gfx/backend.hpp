#ifndef SFW_BACKEND_HPP
#define SFW_BACKEND_HPP

//----------------------------------------------------------------------------
// Dispatch backend-specific #includes
//
//https://stackoverflow.com/a/50728314/1479945

#ifndef SFW_GFX_BACKEND
//!!Should only be a warning about including some default!
//!!But #warning is c++23(!...), and isn't supported by MSVC yet. :-/
//!!(Note that GCC has supported it for quite a long time.)
//!!#warning Backend was not selected. Use: #include "sfw/cfg/USE_{backendname}"
//!!#warning Selecting SFML as default graphics backend...
#include "sfw/cfg/USE_SFML"
#endif

#define STR_IMPL(A) #A
#define STR(A) STR_IMPL(A)
#define GFX_BACKEND_SPECIFIC(filename) STR(sfw/Gfx/_backend-specific/SFW_GFX_BACKEND/filename)
// Usage: #include GFX_BACKEND_SPECIFIC(headername.h)


#endif // SFW_BACKEND_HPP
