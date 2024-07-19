#include "sfw/gfx/element/Arrow.hpp"

#include "sfw/Theme.hpp"
#include "SAL/geometry/Rectangle.hpp"

#include <cmath>


namespace sfw//!!::parts
{
using namespace SAL::geometry;

Arrow::Arrow(Direction direction) :
	m_direction(direction),
	m_frame(fRect(Theme::getArrowTextureRect()))
{
/*!!OLD:
	const auto& rect = fRect(Theme::getArrowTextureRect());
	auto left   = rect.left();
	auto top    = rect.top();
	auto width  = rect.width();
	auto height = rect.height();

	//!!?? How about rounding, like in setPosition()?!
	m_vertices[0].texCoords = sf::Vector2f(left, top);                  //!! Direct SFML API call!
	m_vertices[1].texCoords = sf::Vector2f(left, top + height);         //!! Direct SFML API call!
	m_vertices[2].texCoords = sf::Vector2f(left + width, top);          //!! Direct SFML API call!
	m_vertices[3].texCoords = sf::Vector2f(left + width, top + height); //!! Direct SFML API call!

	m_frame.texture_rect(Theme::getArrowTextureRect()); //!!?? I think this one copies. Adapt Theme to avoid that?!
!!*/
	updateGeometry({}, direction);
}


void Arrow::setFillColor([[maybe_unused]] Color color)
{
//!! -> #97: "...mysterious overpaint with setFillColor"
//!!
//!!    for (int i = 0; i < 4; ++i)
//!!        m_vertices[i].color = color;
}


void Arrow::setPosition(fVec2 pos)
{
	//! We call updateGeometry() here, but not at move(), because abs. positions
	//! need to be set differently based on the orientation, unlike with move()!
	updateGeometry(pos.round(), m_direction); // OpenGL could render things blurry without rounding!
}


void Arrow::move(fVec2 delta)
{
	return m_frame.move(delta/*!!??.round()??!!*/);
}


fVec2 Arrow::getSize() const
{
	return fVec2(m_frame.texture_size());
		//!! Not m_frame.size(): that's not even initialized to non-0...
		//!! TextureFrame is not the best Impl for this!...
}


void Arrow::draw(const SAL::gfx::RenderContext& ctx) const
{
	m_frame.draw(ctx, Theme::getTexture());
}


void Arrow::updateGeometry(fVec2 pos, Direction direction)
{
	m_frame.place(pos, (SAL::gfx::TextureFrame::Direction)(int)direction); //!! static_assert that the layouts match!
}


} // namespace sfw//!!::parts
