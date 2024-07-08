#ifndef _OIU345YTNDC84675TOB678WTBN5MC94285N7_
#define _OIU345YTNDC84675TOB678WTBN5MC94285N7_


#include "SAL/gfx/Render.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>


namespace SAL::gfx::adapter
{
namespace SFML
{

//!! This entire hocus pocus with the template is not used yet,
//!! as the generic layer is empty now (just a `using` alias)!
//!! Still keeping it here as a reminder. We need generic vector/color etc. types
//!! before proceeding to flash out the generic layer...

#define Self FilledRect_Impl //<FilledRect>
//!!template <class FilledRect> class Self
struct FilledRect_Impl : public SAL::gfx::Drawable
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

	//----------------------------------------------------------------------------
public: //! <- NOT private, because draw() may be accessed directly (statically),
        //!    rather than just polymorphically (dynamically) via a pointer!
	void draw(const SAL::gfx::RenderContext& ctx) const override
	{
		auto lstates = ctx.props;

		sf::RectangleShape r(size);
		r.setPosition(position);
		r.setFillColor(colorFill);
		r.setOutlineColor(colorBorder);
		r.setOutlineThickness(1);

		ctx.target.draw(r, lstates);
	}

}; // class FilledRect_Impl
#undef Self

} // namespace SFML


// Save the current type to a backend-independent symbol:
using FilledRect_Impl = SFML::FilledRect_Impl; //!! Use a "standard" SAL macro for the indirectly name the current backend!


} // namespace SAL::gfx::adapter


#endif // _OIU345YTNDC84675TOB678WTBN5MC94285N7_
