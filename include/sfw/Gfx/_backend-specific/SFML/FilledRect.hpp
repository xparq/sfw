#ifndef SFW_GFX_FILLEDRECT_SFML_HPP
#define SFW_GFX_FILLEDRECT_SFML_HPP

#include "sfw/Gfx/Render.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>

namespace sfw {

//!! This entire hocus pocus with the template is not used yet,
//!! as the generic layer is empty now (just a `using` alias)!
//!! Still keeping it here as a reminder. We need generic vector/color etc. types
//!! before proceeding to flash out the generic layer...

#define Self FilledRect_Impl //<FilledRect>
//!!template <class FilledRect> class Self
struct FilledRect_Impl : public gfx::Drawable
{
	sf::Vector2f size{};
	sf::Vector2f position{};
	sf::Color colorFill{};
	sf::Color colorBorder{};
	//!!?? It's a trade-off between size (the rather fat SFML Rect. (Shape) object here)
	//!!?? and speed (reinitializing the rect. from scratch in each frame, in draw())...
	//!!?? sf::RectangleShape m_rect;

	Self() = default;

	Self(const sf::Vector2f& s, sf::Color fill_color, sf::Color border_color):
		size(s), //! size(size) makes GCC freak out...
		colorFill(fill_color),
		colorBorder(border_color)
	{}

	Self(const sf::Vector2f& s, sf::Color fill_color):
		Self(s, fill_color, fill_color)
	{}

protected:
	//----------------------------------------------------------------------------
	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override
	{
		auto lstates = states;

		sf::RectangleShape r(size);
		r.setPosition(position);
		r.setFillColor(colorFill);
		r.setOutlineColor(colorBorder);
		r.setOutlineThickness(1);

		target.draw(r, lstates);
	}

//!!#undef Self
}; // class

} // namespace

#endif // SFW_GFX_FILLEDRECT_SFML_HPP
