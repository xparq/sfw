#ifndef SFW_GFX_TEXT_SFML_HPP
#define SFW_GFX_TEXT_SFML_HPP

#include <SFML/Graphics/Text.hpp>

namespace sfw
{

struct Text : sf::Text
{
	Text() : sf::Text(Theme::getFont()) {}
};

} // namespace

#endif // SFW_GFX_TEXT_SFML_HPP
