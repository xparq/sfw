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

Image::Image(const SAL::gfx::Texture& texture, const geometry::iRect& r): Image()
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

Image* Image::setTexture(const SAL::gfx::Texture& texture, const geometry::iRect& crop)
{
	// Don't copy over itself (but still alow cropping even then)
	if (&m_texture != &texture)
	{
		m_texture = texture;
	}

	// Set the "crop window" to the full native texture size if crop == Null
	setCropRect(crop == NullRect ? iRect({}, m_texture.size())
	                             : crop);
	return this;
}


Image* Image::setCropRect(const geometry::iRect& r)
{
	int left   = r.left()  ? r.left()   : 0;
	int top    = r.top()   ? r.top()    : 0;
	int width  = r.width() ? r.width()  : m_texture.size().x();
	int height = r.width() ? r.height() : m_texture.size().y();

	if (width  > m_texture.size().x()) width  = m_texture.size().x();
	if (height > m_texture.size().y()) height = m_texture.size().y();

	m_baseSize = {width, height};

	// Update texture "crop window"
	m_vertices[0].texture_position({left        , top});
	m_vertices[1].texture_position({left        , top + height});
	m_vertices[2].texture_position({left + width, top});
	m_vertices[3].texture_position({left + width, top + height});

	setSize(m_baseSize * m_scalingFactor);
	return this;
}

iRect Image::cropRect() const
{
	return iRect(m_vertices[0].texture_position(),
	             m_vertices[3].texture_position() - m_vertices[0].texture_position());
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
		m_vertices[i].color(color);
	return this;
}


void Image::onResized()
{
	//!! Doesn't work with my Vector class yet:
	//!!auto [right, bottom] = getSize();
	auto size = getSize();
	auto right = size.x(), bottom = size.y();

	m_vertices[0].position({0, 0});
	m_vertices[1].position({0, bottom});
	m_vertices[2].position({right, 0});
	m_vertices[3].position({right, bottom});
}


void Image::draw(const SAL::gfx::RenderContext& ctx) const
{
	auto ctx_mod = ctx;
	ctx_mod.props.transform *= getTransform(); //!! Direct SFML use!
	SAL::gfx::TexturedVertex2::draw_strip(ctx_mod, m_texture, m_vertices, 4);
/*!! OLD:
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	sfml_renderstates.texture = &m_texture;
	ctx.target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
!!*/
}


} // namespace sfw
