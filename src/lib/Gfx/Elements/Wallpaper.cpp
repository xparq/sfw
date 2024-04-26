#include "sfw/Gfx/Elements/Wallpaper.hpp"
#include "sfw/util/diagnostics.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace sfw
{

Wallpaper::Wallpaper()
{
}

Wallpaper::Wallpaper(const std::string& filename, const sf::IntRect& r): Wallpaper()
{
	setImage(filename, r);
}

Wallpaper::Wallpaper(const sf::Image& image, const sf::IntRect& r): Wallpaper()
{
	setImage(image, r);
}

Wallpaper::Wallpaper(const sf::Texture& texture, const sf::IntRect& r): Wallpaper()
{
	setImage(texture, r);
}


Wallpaper* Wallpaper::setImage(const std::string& filename, const sf::IntRect& r)
{
	if (m_texture.loadFromFile(filename, r))
	{
		setImage(m_texture);
	}
	// else: SFML has already written an error, but we should add our own error feedback here!!

	return this;
}

Wallpaper* Wallpaper::setImage(const sf::Image& image, const sf::IntRect& r)
{
	if (m_texture.loadFromImage(image, r)) //!!?? What does SFML do with a null/invalid rect?!
	{
		setImage(m_texture);
	}
	return this;
}

Wallpaper* Wallpaper::setImage(const sf::Texture& texture, const sf::IntRect& crop)
{
	// Don't copy over itself (but still alow cropping even then)
	if (&m_texture != &texture)
	{
		m_texture = texture;
	}

	// Set the "crop window" to the full native texture size if crop == Null
	setCropRect(crop == NullRect ? sf::IntRect{{0, 0}, {(int)m_texture.getSize().x, (int)m_texture.getSize().y}}
	                             : crop);
	return this;
}


Wallpaper* Wallpaper::setCropRect(const sf::IntRect& r)
{
	float left = (float) (r.left ? r.left : 0);
	float top  = (float) (r.top  ? r.top  : 0);
	float width  = (float) (r.width ? r.width  : m_texture.getSize().x);
	float height = (float) (r.width ? r.height : m_texture.getSize().y);

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

sf::IntRect Wallpaper::getCropRect() const
{
	return sf::IntRect(sf::Vector2i(m_vertices[0].texCoords),
	                   sf::Vector2i(m_vertices[3].texCoords - m_vertices[0].texCoords));
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


Wallpaper* Wallpaper::setSize(sf::Vector2f size)
{
	m_vertices[0].position = {0, 0};
	m_vertices[1].position = {0, size.y};
	m_vertices[2].position = {size.x, 0};
	m_vertices[3].position = {size.x, size.y};

	return this;
}

sf::Vector2f Wallpaper::getSize() const
{
	return { m_vertices[3].position.x - m_vertices[0].position.x,
	         m_vertices[3].position.y - m_vertices[0].position.y };
}


void Wallpaper::draw(const gfx::RenderContext& ctx) const
{
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	sfml_renderstates.texture = &m_texture;
	ctx.target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
}

} // namespace sfw
