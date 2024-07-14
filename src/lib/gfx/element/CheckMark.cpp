#include "sfw/gfx/element/CheckMark.hpp"

#include "sfw/Theme.hpp"
#include "sfw/geometry/Rectangle.hpp"

#include <cmath>


namespace sfw
{
using namespace geometry;

CheckMark::CheckMark()
{
 	const iRect rect = Theme::getCheckMarkTextureRect();

	m_vertices[0].texture_position({rect.left()               , rect.top()});
	m_vertices[1].texture_position({rect.left()               , rect.top() + rect.height()});
	m_vertices[2].texture_position({rect.left() + rect.width(), rect.top()});
	m_vertices[3].texture_position({rect.left() + rect.width(), rect.top() + rect.height()});

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
//!! Can't:	m_vertices[i].position += delta;
//!! or even:	m_vertices[i].position() += delta;
		m_vertices[i].position(m_vertices[i].position() + delta);
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


void CheckMark::setColor(Color c)
{
	for (int i = 0; i < 4; ++i)
		m_vertices[i].color(c);
}


void CheckMark::draw(const gfx::RenderContext& ctx) const // override
{
	gfx::TexturedVertex2::draw_trianglestrip(ctx, Theme::getTexture(), m_vertices, 4);
}


void CheckMark::updateGeometry(float x, float y)
{
	const auto rect = fRect(Theme::getCheckMarkTextureRect()); // iRect -> fRect
	m_vertices[0].position( {x               , y} );
	m_vertices[1].position( {x               , y + rect.height()} );
	m_vertices[2].position( {x + rect.width(), y} );
	m_vertices[3].position( {x + rect.width(), y + rect.height()} );
}


} // namespace sfw
