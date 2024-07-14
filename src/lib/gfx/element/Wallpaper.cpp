#include "sfw/gfx/element/Wallpaper.hpp"

#include "SAL/util/diagnostics.hpp"

#include <SFML/Graphics/RenderStates.hpp> //!! .transform *= getTransform()


namespace sfw
{
using namespace geometry;


Wallpaper::Wallpaper()
{
}

Wallpaper::Wallpaper(std::string_view filename, const iRect& r): Wallpaper()
{
	setImage(filename, r);
}

Wallpaper::Wallpaper(const sf::Image& image, const iRect& r): Wallpaper()
{
	setImage(image, r);
}

Wallpaper::Wallpaper(const SAL::gfx::Texture& texture, const iRect& r): Wallpaper()
{
	setImage(texture, r);
}


Wallpaper* Wallpaper::setImage(std::string_view filename, const iRect& r)
{
	if (m_texture.load(filename, r))
	{
		setImage(m_texture);
	}
	// else: SFML has already written an error, but we should add our own error feedback here!!

	return this;
}

Wallpaper* Wallpaper::setImage(const sf::Image& image, const iRect& r)
{
	if (m_texture.set(image, r)) //!!?? What does SFML do with a null/invalid rect?!
	{
		setImage(m_texture);
	}
	return this;
}

Wallpaper* Wallpaper::setImage(const SAL::gfx::Texture& texture, const iRect& crop)
{
	// Don't copy over itself (but still alow cropping even then)
	if (&m_texture != &texture)
	{
		m_texture = texture;
	}

	// Set the "crop window" to the full native texture size by default (if crop == Null):
	setCropRect(crop == NullRect ? iRect({}, m_texture.size())
	                             : crop);
	return this;
}


Wallpaper* Wallpaper::setCropRect(const iRect& r)
{
	int left   = r.left()  ? r.left()   : 0;
	int top    = r.top()   ? r.top()    : 0;
	int width  = r.width() ? r.width()  : m_texture.size().x();
	int height = r.width() ? r.height() : m_texture.size().y();

	if (width  > m_texture.size().x()) width  = m_texture.size().x();
	if (height > m_texture.size().y()) height = m_texture.size().y();

	m_baseSize = {width, height};

	// Update texture "crop window"
	m_vertices[0].texture_position({left         , top});
	m_vertices[1].texture_position({left         , top + height});
	m_vertices[2].texture_position({left + width, top});
	m_vertices[3].texture_position({left + width, top + height});

	setSize(m_baseSize * m_scalingFactor);
	return this;
}

iRect Wallpaper::getCropRect() const
{
	return {m_vertices[0].texture_position(),
	        m_vertices[3].texture_position() - m_vertices[0].texture_position()};
}


Wallpaper* Wallpaper::scale(float factor)
{
	m_scalingFactor = factor;
	setSize(m_baseSize * m_scalingFactor);
	return this;
}

Wallpaper* Wallpaper::rescale(float factor)
{
	m_scalingFactor *= factor;
	setSize(getSize() * m_scalingFactor);
	return this;
}


Wallpaper* Wallpaper::setColor(const Color& color)
{
	for (int i = 0; i < 4; ++i)
		m_vertices[i].color(color);
	return this;
}

Color Wallpaper::getColor() const
{
	return m_vertices[0].color(); //!! Not the overwhelmingly solid/future-proof approach I used to dream of as a child...
}


Wallpaper* Wallpaper::setSize(iVec2 size)
{
	m_vertices[0].position( {0,               0} );
	m_vertices[1].position( {0,               (float)size.y()} );
	m_vertices[2].position( {(float)size.x(), 0} );
	m_vertices[3].position( {(float)size.x(), (float)size.y()} );

	return this;
}

iVec2 Wallpaper::getSize() const
{
//!! OLD:
	return { (int)m_vertices[3].position().x() - (int)m_vertices[0].position().x(),
	         (int)m_vertices[3].position().y() - (int)m_vertices[0].position().y() };
//!! USE THIS, if Vec2 type conversions get implemented:
//!!	
//!!	return iVec2(m_vertices[3].position() - m_vertices[0].position()); //! The explicit iVec conversion is optional, actually.
}


void Wallpaper::draw(const SAL::gfx::RenderContext& ctx) const
{
	auto ctx_mod = ctx;
	ctx_mod.props.transform *= getTransform(); //!! Direct SFML use!
	SAL::gfx::TexturedVertex2::draw_trianglestrip(ctx_mod, m_texture, m_vertices, 4);
}


} // namespace sfw
