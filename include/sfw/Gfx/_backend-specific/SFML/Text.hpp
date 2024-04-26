#ifndef _8CC78TY678W78F49W678TNT62X438T6EW4T78CV456_
#define _8CC78TY678W78F49W678TNT62X438T6EW4T78CV456_

#include "sfw/Gfx/Render.hpp"

#include <SFML/Graphics/Text.hpp>
#include "sfw/util/shim/sfml.hpp"

namespace sfw
{

class Text : public sf::Text, public gfx::Drawable
{
public:
	Text(const std::string& s = "", unsigned int height = 30)
		: sf::Text(Theme::getFont(), stdstring_to_SFMLString(s), height) {}

	void set(const std::string& str) { setString(stdstring_to_SFMLString(str)); }
	std::string get() const { return SFMLString_to_stdstring(getString()); }

	void draw(const gfx::RenderContext& ctx) const override { ctx.target.draw((sf::Text&)*this, ctx.props); }
};

} // namespace sfw

#endif // _8CC78TY678W78F49W678TNT62X438T6EW4T78CV456_
