#ifndef GUI_IMAGE_HPP
#define GUI_IMAGE_HPP

#include "../Widget.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <string>

namespace gui
{

/**
 * Widget for displaying a picture
 * Passive widget: cannot be focused or trigger event
 */
class Image: public Widget
{
public:
    Image();
    Image(const std::string& filename, const sf::IntRect& r = {{0,0}, {0,0}});
    Image(const sf::Image& image);
    Image(const sf::Texture& texture);

    void setTexture(const std::string& filename, const sf::IntRect& r = {{0,0}, {0,0}});
    void setTexture(const sf::Image& image);
    void setTexture(const sf::Texture& texture);

    const sf::Texture& texture() const { return m_texture; }
          sf::Texture& texture()       { return m_texture; }

    void setCropRect(const sf::IntRect& r);
    sf::IntRect getCropRect() const;

    void setColor(const sf::Color& color);

private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    sf::Vertex m_vertices[4];
    sf::Texture m_texture;
};

}

#endif // GUI_IMAGE_HPP
