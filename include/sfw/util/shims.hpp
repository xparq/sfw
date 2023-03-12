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
#define GFX_BACKEND_SPECIFIC(filename) STR(sfw/Gfx/_backend-specific/SFW_GFX_BACKEND/filename)
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



#include <SFML/System/String.hpp>
#include <string>

//!! This would require an iterator to `end` -- but if they mean an utf-8 iterator,
//!! rather than byte (`char`), then I can't give it to them this way, here, now...
//!! (The API ref. doesn't say what type it should be, it's just a template param...)
//!!    m_box.item().setString(sf::String::fromUtf8(text));
//!!
//!! So, let's just stick to the "utf-8 everywhere" principle, and do:
//!!    m_box.item().setString(text);
//!!
//!! Except... That didn't seem to work with SFML... :-/ Same broken decoding. WTH?!
//!! OK, *sigh*, still trying the original choice then...
//!! But that would probably crash for char-length >= utf8-length! :(
inline sf::String stdstring_to_SFMLString(const std::string& stdstr)
{
    return sf::String::fromUtf8(stdstr.c_str(), stdstr.c_str() + stdstr.length());
    //! Wow, OK, it didn't crash, and even seems to work fine! :-o
    //! So they do need plain "byte iterators" for this then?! Phew...
}   //! (Can't quite make sense of that, but whatever -- let's celebrate! :) )

//!!See above about not trying to be too utf8-smart for nothing...
inline std::string SFMLString_to_stdstring(const sf::String& sfstr)
{
    return (const char*)sfstr.toUtf8().c_str();
}

#endif // SFW_SHIMS_HPP
