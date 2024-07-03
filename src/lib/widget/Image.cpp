#include "sfw/widget/Image.hpp"

#include <SFML/Graphics/RenderTarget.hpp>


namespace sfw
{
using namespace geometry;


Image::Image()
{
	setFocusable(false);
}

Image::Image(std::string_view filename, const geometry::iRect& r): Image()
{
	setTexture(filename, r);
}

Image::Image(const sf::Image& image, const geometry::iRect& r): Image()
{
	setTexture(image, r);
}

Image::Image(const /*!!gfx::!!*/Texture& texture, const geometry::iRect& r): Image()
{
	setTexture(texture, r);
}


Image* Image::setTexture(std::string_view filename, const geometry::iRect& r)
{
	if (m_texture.load(filename, r))
	{
		setTexture(m_texture);
	}
	// else: SFML has already written an error, but we should add our own error feedback here!!

	return this;
}

Image* Image::setTexture(const sf::Image& image, const geometry::iRect& r)
{
	if (m_texture.set(image, r)) //!!?? What does SFML do with a null/invalid ("negative") rect?!
	{
		setTexture(m_texture);
	}
	return this;
}

Image* Image::setTexture(const /*!!gfx::!!*/Texture& texture, const geometry::iRect& crop)
{
	// Don't copy over itself (but still alow cropping even then)
	if (&m_texture != &texture)
	{
		m_texture = texture;
	}

	// Set the "crop window" to the full native texture size if crop == Null
	setCropRect(crop == NullRect ? iRect({0, 0}, m_texture.size())
	                             : crop);
	return this;
}


Image* Image::setCropRect(const geometry::iRect& r)
{
	float left = (float) (r.left() ? r.left() : 0);
	float top  = (float) (r.top()  ? r.top()  : 0);
	float width  = (float) (r.width() ? r.width()  : m_texture.size().x());
	float height = (float) (r.width() ? r.height() : m_texture.size().y());

	if (width  > m_texture.size().x()) width  = (float)m_texture.size().x();
	if (height > m_texture.size().y()) height = (float)m_texture.size().y();

	m_baseSize = {width, height};

	// Update texture "crop window"
	m_vertices[0].texCoords = sf::Vector2f(left, top);
	m_vertices[1].texCoords = sf::Vector2f(left, top + height);
	m_vertices[2].texCoords = sf::Vector2f(left + width, top);
	m_vertices[3].texCoords = sf::Vector2f(left + width, top + height);

	setSize(m_baseSize * m_scalingFactor);
	return this;
}

iRect Image::cropRect() const
{
	return iRect(iVec2(m_vertices[0].texCoords),
	             iVec2(m_vertices[3].texCoords - m_vertices[0].texCoords));
}


Image* Image::scale(float factor)
{
	m_scalingFactor = factor;
	setSize(m_baseSize * m_scalingFactor);
	return this;
}


Image* Image::rescale(float factor)
{
	m_scalingFactor *= factor;
	setSize(getSize() * m_scalingFactor);
	return this;
}


Image* Image::setColor(const sf::Color& color)
{
	for (int i = 0; i < 4; ++i)
		m_vertices[i].color = color;
	return this;
}


void Image::onResized()
{
	//!! Doesn't work with my Vector class yet:
	//!!auto [right, bottom] = getSize();
	auto size = getSize();
	auto right = size.x(), bottom = size.y();

	m_vertices[0].position = {0, 0};
	m_vertices[1].position = {0, bottom};
	m_vertices[2].position = {right, 0};
	m_vertices[3].position = {right, bottom};
}


void Image::draw(const gfx::RenderContext& ctx) const
{
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	sfml_renderstates.texture = &m_texture;
	ctx.target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
}


} // namespace sfw
