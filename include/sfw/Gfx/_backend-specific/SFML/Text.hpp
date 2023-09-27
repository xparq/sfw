#ifndef SFW_GFX_TEXT_SFML_HPP
#define SFW_GFX_TEXT_SFML_HPP

#include <SFML/Graphics/Text.hpp>
#include "sfw/util/shim/sfml.hpp"

namespace sfw
{

struct Text : sf::Text
{
	Text(const std::string& s = "", unsigned int height = 30)
		: sf::Text(Theme::getFont(), stdstring_to_SFMLString(s), height) {}

	void set(const std::string& str) { setString(stdstring_to_SFMLString(str)); }
	std::string get() const { return SFMLString_to_stdstring(getString()); }
};

} // namespace

#endif // SFW_GFX_TEXT_SFML_HPP
