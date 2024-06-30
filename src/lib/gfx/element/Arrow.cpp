#include "sfw/gfx/element/Arrow.hpp"

#include "sfw/Theme.hpp"
#include "sfw/gfx/Render.hpp"
#include "sfw/geometry/Rectangle.hpp"

#include <SFML/Graphics/RenderTarget.hpp> // See draw()...

#include <cmath>

namespace sfw
{
using namespace geometry;

Arrow::Arrow(Direction direction) :
	m_direction(direction)
{
	const iRect& rect = Theme::getArrowTextureRect();
	float left   = (float)rect.left();
	float top    = (float)rect.top();
	float width  = (float)rect.width();
	float height = (float)rect.height();

	//!!?? How about rounding, like in setPosition()?!
	m_vertices[0].texCoords = sf::Vector2f(left, top);                  //!! Direct SFML API call!
	m_vertices[1].texCoords = sf::Vector2f(left, top + height);         //!! Direct SFML API call!
	m_vertices[2].texCoords = sf::Vector2f(left + width, top);          //!! Direct SFML API call!
	m_vertices[3].texCoords = sf::Vector2f(left + width, top + height); //!! Direct SFML API call!

	updateGeometry(0, 0, direction);
}


void Arrow::setFillColor([[maybe_unused]] const sf::Color& color)
{
//!! -> #97: "...mysterious overpaint with setFillColor"
//!!
//!!    for (int i = 0; i < 4; ++i)
//!!        m_vertices[i].color = color;
}


void Arrow::setPosition(fVec2 pos)
{
	updateGeometry(roundf(pos.x()), roundf(pos.y()), m_direction); // OpenGL could render things blurry without rounding!
}


void Arrow::move(fVec2 delta)
{
	for (int i = 0; i < 4; ++i)
	{
		m_vertices[i].position.x += delta.x();
		m_vertices[i].position.y += delta.y();
	}
}


fVec2 Arrow::getSize() const
{
//!! This one copies:
	const iRect rect = Theme::getArrowTextureRect();
//!! Adapt Theme to avoid that:
//!!	const iRect& rect = Theme::getArrowTextureRect();

	return fVec2((float)rect.width(), (float)rect.height());
}


void Arrow::draw(const gfx::RenderContext& ctx) const
{
	auto lstates = ctx.props;
	lstates.texture = &Theme::getTexture(); //!! Direct use of the SFML API! :-/
	ctx.target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, lstates); //!! Direct use of the SFML API! :-/
}


void Arrow::updateGeometry(float x, float y, Direction direction)
{
//!! This one copies:
	const iRect rect = Theme::getArrowTextureRect();
//!! Adapt Theme to avoid that:
//!!	const iRect& rect = Theme::getArrowTextureRect();
	auto width  = (float)rect.width();
	auto height = (float)rect.height();

	switch (direction)
	{
	case Top:
		m_vertices[0].position = sf::Vector2f(x, y);
		m_vertices[1].position = sf::Vector2f(x, y + height);
		m_vertices[2].position = sf::Vector2f(x + width, y);
		m_vertices[3].position = sf::Vector2f(x + width, y + height);
		break;
	case Bottom:
		m_vertices[0].position = sf::Vector2f(x + width, y + height);
		m_vertices[1].position = sf::Vector2f(x + width, y);
		m_vertices[2].position = sf::Vector2f(x, y + height);
		m_vertices[3].position = sf::Vector2f(x, y);
		break;
	case Left:
		m_vertices[0].position = sf::Vector2f(x, y + height);
		m_vertices[1].position = sf::Vector2f(x + width, y + height);
		m_vertices[2].position = sf::Vector2f(x, y);
		m_vertices[3].position = sf::Vector2f(x + width, y);
		break;
	case Right:
		m_vertices[0].position = sf::Vector2f(x + width, y);
		m_vertices[1].position = sf::Vector2f(x, y);
		m_vertices[2].position = sf::Vector2f(x + width, y + height);
		m_vertices[3].position = sf::Vector2f(x, y + height);
		break;
	}
	m_direction = direction;
}

} // namespace sfw
