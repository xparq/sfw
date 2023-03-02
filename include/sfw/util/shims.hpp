#ifndef SFW_SHIMS_HPP
#define SFW_SHIMS_HPP

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
#define GFX_BACKEND_SPECIFIC(filename) STR(sfw/Gfx/backend-specific/SFW_GFX_BACKEND/filename)
// Usage: #include GFX_BACKEND_SPECIFIC(headername.h)


//----------------------------------------------------------------------------
// This should also be moved to something SFML-specific:
//
#include <SFML/Window/Event.hpp>
// FFS, still no default == for POD structs, even in c++20?! :-o :-/
inline bool operator==(const sf::Event::KeyEvent& key1, const sf::Event::KeyEvent& key2)
{
    return key1.code == key2.code
      //&& key1.scancode == key2.scancode
        && key1.alt == key2.alt
        && key1.control == key2.control
        && key1.shift == key2.shift
        && key1.system == key2.system
	;
}

#endif // SFW_SHIMS_HPP
