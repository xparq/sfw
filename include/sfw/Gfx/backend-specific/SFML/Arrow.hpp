#ifndef GUI_ARROW_SFML_HPP
#define GUI_ARROW_SFML_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace sfw
{

class Arrow: public sf::Drawable
{
public:
    enum Direction
    {
        Left,
        Right,
        Top,
        Bottom
    };

    Arrow(Direction direction);

    void setFillColor(const sf::Color& color);

    void move(sf::Vector2f delta);

    void setPosition(sf::Vector2f pos);

    sf::Vector2f getSize() const;

private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;

    void updateGeometry(float x, float y, Direction direction);

    sf::Vertex m_vertices[4];
    Direction m_direction;
};

} // namespace

#endif // GUI_ARROW_SFML_HPP
