#include "sfw/gfx/element/CheckMark.hpp"

#include "sfw/Theme.hpp"
#include "sfw/geometry/Rectangle.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace sfw
{
using namespace geometry;

CheckMark::CheckMark()
{
 	const auto rect = fRect(Theme::getCheckMarkTextureRect()); // iRect -> fRect

	m_vertices[0].texCoords = sf::Vector2f(rect.left(), rect.top());
	m_vertices[1].texCoords = sf::Vector2f(rect.left(), rect.top() + rect.height());
	m_vertices[2].texCoords = sf::Vector2f(rect.left() + rect.width(), rect.top());
	m_vertices[3].texCoords = sf::Vector2f(rect.left() + rect.width(), rect.top() + rect.height());

	updateGeometry(0, 0);
}


void CheckMark::setPosition(fVec2 pos)
{
	updateGeometry(roundf(pos.x()), roundf(pos.y())); // OpenGL could render things blurred without the rounding!
}


void CheckMark::move(fVec2 delta)
{
	for (int i = 0; i < 4; ++i)
	{
		m_vertices[i].position.x += delta.x();
		m_vertices[i].position.y += delta.y();
	}
}


void CheckMark::setSize(float)
{
	//!! NOT IMPLEMENTED
}


fVec2 CheckMark::getSize() const
{
	const auto rect = fRect(Theme::getCheckMarkTextureRect()); // iRect -> fRect
	return rect.size();
}


void CheckMark::setColor(const sf::Color& color)
{
	for (int i = 0; i < 4; ++i)
		m_vertices[i].color = color;
}


void CheckMark::draw(const gfx::RenderContext& ctx) const // override
{
	auto lstates = ctx.props;
	lstates.texture = &Theme::getTexture();
	ctx.target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, lstates);
}


void CheckMark::updateGeometry(float x, float y)
{
	const auto rect = fRect(Theme::getCheckMarkTextureRect()); // iRect -> fRect
	m_vertices[0].position = sf::Vector2f(x, y);
	m_vertices[1].position = sf::Vector2f(x, y + rect.height());
	m_vertices[2].position = sf::Vector2f(x + rect.width(), y);
	m_vertices[3].position = sf::Vector2f(x + rect.width(), y + rect.height());
}

} // namespace sfw
