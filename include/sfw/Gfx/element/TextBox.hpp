#ifndef SFW_GFX_TEXT_HPP
#define SFW_GFX_TEXT_HPP
//----------------------------------------------------------------------------
// 2c99b33@SFML removed the default sf::Text ctor with no font.
// This helper class can be used to help existing APIs continue to rely
// on the default font implicitly.

#include "sfw/gfx/Text.hpp"

#include "sfw/gfx/backend.hpp"
#include GFX_BACKEND_SPECIFIC(TextBox.hpp)

#endif // SFW_GFX_TEXT_HPP
