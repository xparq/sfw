#ifndef SFW_GFX_TEXT_SFML_HPP
#define SFW_GFX_TEXT_SFML_HPP

#include <SFML/Graphics/Text.hpp>
#include "sfw/util/shims.hpp"

namespace sfw
{

struct Text : sf::Text
{
	Text() : sf::Text(Theme::getFont()) {}

	void set(const std::string& str) { setString(stdstring_to_SFMLString(str)); }
	std::string get() const { return SFMLString_to_stdstring(getString()); }
};

} // namespace

#endif // SFW_GFX_TEXT_SFML_HPP
