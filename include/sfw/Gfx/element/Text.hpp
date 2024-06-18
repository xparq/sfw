#ifndef SFW_GFX_TEXT_HPP
#define SFW_GFX_TEXT_HPP
//----------------------------------------------------------------------------
// 2c99b33@SFML removed the default sf::Text ctor with no font.
// This helper class can be used to help existing APIs continue to rely
// on the default font implicitly.

#include "sfw/Theme.hpp" //!!?? For the "current font"; But could it not depend on the Theme actually?

#include "sfw/gfx/backend.hpp"
#include GFX_BACKEND_SPECIFIC(Text.hpp)

#endif // SFW_GFX_TEXT_HPP
