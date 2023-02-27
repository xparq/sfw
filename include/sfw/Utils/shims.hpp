#ifndef _SFW_SHIMS_HPP_
#define _SFW_SHIMS_HPP_

#include <SFML/Window/Event.hpp>

 // FFS, still no default == for POD structs, even in c++20?! :-o :-/
bool operator==(const sf::Event::KeyEvent& key1, const sf::Event::KeyEvent& key2)
{
    return key1.code == key2.code
      //&& key1.scancode == key2.scancode
        && key1.alt == key2.alt
        && key1.control == key2.control
        && key1.shift == key2.shift
        && key1.system == key2.system
	;
}

#endif // _SFW_SHIMS_HPP_
