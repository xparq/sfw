#include "sfw/Widgets/Image.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace sfw
{

Image::Image()
{
    setSelectable(false);
}

Image::Image(const std::string& filename, const sf::IntRect& r): Image()
{
    setTexture(filename, r);
}

Image::Image(const sf::Image& image/*!!, const sf::IntRect& crop = nullrect!!*/): Image()
{
    setTexture(image);
}

Image::Image(const sf::Texture& texture/*!!, const sf::IntRect& crop = nullrect!!*/): Image()
{
    setTexture(texture);
}


void Image::setTexture(const std::string& filename, const sf::IntRect& r)
{
    if (!m_texture.loadFromFile(filename, r))
    {
        // SFML has already written an error.
        return;
    }
    setTexture(m_texture);
}

void Image::setTexture(const sf::Image& image)
{
    m_texture.update(image);
    setTexture(m_texture);
}

void Image::setTexture(const sf::Texture& texture)
{
    // Don't copy over itself (but still alow cropping even then)
    if (&m_texture != &texture)
    {
        m_texture = texture;
    }

    setCropRect({{0, 0}, {(int)m_texture.getSize().x, (int)m_texture.getSize().y}});
}


void Image::setCropRect(const sf::IntRect& r)
{
    float left = (float) (r.left ? r.left : 0);
    float top  = (float) (r.top  ? r.top  : 0);
    float width  = (float) (r.width ? r.width  : m_texture.getSize().x);
    float height = (float) (r.width ? r.height : m_texture.getSize().y);

    if (width  > m_texture.getSize().x) width  = (float)m_texture.getSize().x;
    if (height > m_texture.getSize().y) height = (float)m_texture.getSize().y;

    // Set texture crop "window" rect
    m_vertices[0].texCoords = sf::Vector2f(left, top);
    m_vertices[1].texCoords = sf::Vector2f(left, top + height);
    m_vertices[2].texCoords = sf::Vector2f(left + width, top);
    m_vertices[3].texCoords = sf::Vector2f(left + width, top + height);

    // Set widget geometry
    m_vertices[0].position = sf::Vector2f(0, 0);
    m_vertices[1].position = sf::Vector2f(0, height);
    m_vertices[2].position = sf::Vector2f(width, 0);
    m_vertices[3].position = sf::Vector2f(width, height);

    setSize(width, height);
}

sf::IntRect Image::getCropRect() const
{
    return sf::IntRect(sf::Vector2i(m_vertices[0].texCoords),
        sf::Vector2i(m_vertices[3].texCoords - m_vertices[0].texCoords));
}

void Image::setColor(const sf::Color& color)
{
    for (int i = 0; i < 4; ++i)
        m_vertices[i].color = color;
}


void Image::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    sfml_renderstates.texture = &m_texture;
    ctx.target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
#ifdef DEBUG
//    Widget::draw_outline({ctx.target, sfml_renderstates}); // Not the original, untransformed ctx.props!
#endif
}

} // namespace
