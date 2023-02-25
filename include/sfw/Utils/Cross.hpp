#ifndef GUI_CROSS_HPP
#define GUI_CROSS_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace gui
{

class Cross: public sf::Drawable
{
public:
    Cross();

    void setPosition(sf::Vector2f pos);
    void move(sf::Vector2f delta);

    void setSize(float size);
    sf::Vector2f getSize() const;
    void setColor(const sf::Color& color);

private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;

    void updateGeometry(float x, float y);

    sf::Vertex m_vertices[4];
};

}

#endif // GUI_CROSS_HPP
