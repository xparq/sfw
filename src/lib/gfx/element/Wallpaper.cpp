#include "sfw/gfx/element/Wallpaper.hpp"
#include "sfw/util/diagnostics.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

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

Wallpaper::Wallpaper(const sfw::Texture& texture, const iRect& r): Wallpaper()
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

Wallpaper* Wallpaper::setImage(const sfw::Texture& texture, const iRect& crop)
{
	// Don't copy over itself (but still alow cropping even then)
	if (&m_texture != &texture)
	{
		m_texture = texture;
	}

	// Set the "crop window" to the full native texture size if crop == Null
	setCropRect(crop == NullRect ? iRect(iVec2{0, 0}, iVec2{(int)m_texture.getSize().x, (int)m_texture.getSize().y}) //!! Direct use of the SFML API! Also: sf::Texture.getSize() is unsigned...
	                             : crop);

	//!! This braced version failed to compile, BTW:
	//!!setCropRect(crop == NullRect ? iRect{iVec2{0, 0}, iVec2{(int)m_texture.getSize().x, (int)m_texture.getSize().y}} : crop);

	return this;
}


Wallpaper* Wallpaper::setCropRect(const iRect& r)
{
	float left = (float) (r.left() ? r.left() : 0);
	float top  = (float) (r.top()  ? r.top()  : 0);
	float width  = (float) (r.width() ? r.width()  : m_texture.getSize().x);
	float height = (float) (r.width() ? r.height() : m_texture.getSize().y);

	if (width  > m_texture.getSize().x) width  = (float)m_texture.getSize().x;
	if (height > m_texture.getSize().y) height = (float)m_texture.getSize().y;

	m_baseSize = {width, height};

	// Update texture "crop window"
	m_vertices[0].texCoords = sf::Vector2f(left, top);
	m_vertices[1].texCoords = sf::Vector2f(left, top + height);
	m_vertices[2].texCoords = sf::Vector2f(left + width, top);
	m_vertices[3].texCoords = sf::Vector2f(left + width, top + height);

	setSize(m_baseSize * m_scalingFactor);
	return this;
}

iRect Wallpaper::getCropRect() const
{
	return iRect(iVec2(m_vertices[0].texCoords), iVec2(m_vertices[3].texCoords - m_vertices[0].texCoords)); //!! Implicit conversion from SFML Vector2!
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


Wallpaper* Wallpaper::setColor(const sf::Color& color)
{
	for (int i = 0; i < 4; ++i)
		m_vertices[i].color = color;
	return this;
}

sf::Color Wallpaper::getColor() const
{
	return m_vertices[0].color; //! Well, not an overwhelmingly future-proof approach, but OK...
}


Wallpaper* Wallpaper::setSize(iVec2 size)
{
	m_vertices[0].position = {0, 0};
	m_vertices[1].position = {0, (float)size.y()};
	m_vertices[2].position = {(float)size.x(), 0};
	m_vertices[3].position = {(float)size.x(), (float)size.y()};

	return this;
}

iVec2 Wallpaper::getSize() const
{
	return { (int)m_vertices[3].position.x - (int)m_vertices[0].position.x,
	         (int)m_vertices[3].position.y - (int)m_vertices[0].position.y };
//!! USE THIS (+ conv.):
//!!	return m_vertices[3].position - m_vertices[0].position;
}


void Wallpaper::draw(const gfx::RenderContext& ctx) const
{
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	sfml_renderstates.texture = &m_texture.native();
	ctx.target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
}

} // namespace sfw
