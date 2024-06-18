#ifndef GUI_IMAGE_HPP
#define GUI_IMAGE_HPP

#include "sfw/Widget.hpp"

#include <string>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>

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
    Image(const sf::Image& image,      const sf::IntRect& r = NullRect);
    Image(const sf::Texture& texture,  const sf::IntRect& r = NullRect);

    Image* setTexture(const std::string& filename, const sf::IntRect& r = NullRect);
    Image* setTexture(const sf::Image& image,      const sf::IntRect& r = NullRect);
    Image* setTexture(const sf::Texture& texture,  const sf::IntRect& r = NullRect);

    const sf::Texture& texture() const { return m_texture; }

    Image* setCropRect(const sf::IntRect& r);
    sf::IntRect cropRect() const;

    Image* scale(float factor);
    float  scale() const { return m_scalingFactor; }
    // Relative scaling based on the current size, not the original
    Image* rescale(float factor);

    Image* setColor(const sf::Color& color);

private:
    void draw(const gfx::RenderContext& ctx) const override;
    void onResized() override;

    sf::Texture m_texture;
    sf::Vector2f m_baseSize;
    float m_scalingFactor = 1.f;
    sf::Vertex m_vertices[4];
};

} // namespace

#endif // GUI_IMAGE_HPP
