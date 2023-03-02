#ifndef GUI_IMAGE_HPP
#define GUI_IMAGE_HPP

#include "sfw/Widget.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <string>

namespace sfw
{

/**
 * Widget for displaying a picture
 * Passive widget: cannot be focused or trigger event
 */
class Image: public Widget
{
    static constexpr sf::IntRect NullRect = {{0,0}, {0,0}};

public:
    Image();
    Image(const std::string& filename, const sf::IntRect& r = NullRect);
    Image(const sf::Image& image     , const sf::IntRect& r = NullRect);
    Image(const sf::Texture& texture , const sf::IntRect& r = NullRect);

    Image* setTexture(const std::string& filename, const sf::IntRect& r = NullRect);
    Image* setTexture(const sf::Image& image,      const sf::IntRect& r = NullRect);
    Image* setTexture(const sf::Texture& texture,  const sf::IntRect& r = NullRect);

    const sf::Texture& texture() const { return m_texture; }
    sf::Texture& texture()             { return m_texture; }

    Image* setCropRect(const sf::IntRect& r);
    sf::IntRect getCropRect() const;

    Image* rescale(float factor);

    Image* setColor(const sf::Color& color);

private:
    void draw(const gfx::RenderContext& ctx) const override;

    void updateGeometry(float left, float top, float right, float bottom);

    sf::Vertex m_vertices[4];
    sf::Texture m_texture;
};

} // namespace

#endif // GUI_IMAGE_HPP
