#include "sfw/Gfx/Elements/Arrow.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace sfw
{

Arrow::Arrow(Direction direction):
    m_direction(direction)
{
    sf::FloatRect rect = (sf::FloatRect)Theme::getArrowTextureRect();
    m_vertices[0].texCoords = sf::Vector2f(rect.left, rect.top);
    m_vertices[1].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
    m_vertices[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
    m_vertices[3].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);

    updateGeometry(0, 0, direction);
}


void Arrow::setFillColor([[maybe_unused]] const sf::Color& color)
{
//!! -> #97: "...mysterious overpaint with setFillColor"
//!!
//!!    for (int i = 0; i < 4; ++i)
//!!        m_vertices[i].color = color;
}


void Arrow::setPosition(sf::Vector2f pos)
{
    // OpenGL will render things kinda funny otherwise:
    pos.x = roundf(pos.x);
    pos.y = roundf(pos.y);

    updateGeometry(pos.x, pos.y, m_direction);
}


void Arrow::move(sf::Vector2f delta)
{
    for (int i = 0; i < 4; ++i)
    {
        m_vertices[i].position.x += delta.x;
        m_vertices[i].position.y += delta.y;
    }
}


sf::Vector2f Arrow::getSize() const
{
    const sf::IntRect& rect = Theme::getArrowTextureRect();
    return sf::Vector2f((float)rect.width, (float)rect.height);
}


void Arrow::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    auto lstates = states;
    lstates.texture = &Theme::getTexture();
    target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, lstates);
}


void Arrow::updateGeometry(float x, float y, Direction direction)
{
    const sf::IntRect& rect = Theme::getArrowTextureRect();
    switch (direction)
    {
    case Top:
        m_vertices[0].position = sf::Vector2f(x, y);
        m_vertices[1].position = sf::Vector2f(x, y + rect.height);
        m_vertices[2].position = sf::Vector2f(x + rect.width, y);
        m_vertices[3].position = sf::Vector2f(x + rect.width, y + rect.height);
        break;
    case Bottom:
        m_vertices[0].position = sf::Vector2f(x + rect.width, y + rect.height);
        m_vertices[1].position = sf::Vector2f(x + rect.width, y);
        m_vertices[2].position = sf::Vector2f(x, y + rect.height);
        m_vertices[3].position = sf::Vector2f(x, y);
        break;
    case Left:
        m_vertices[0].position = sf::Vector2f(x, y + rect.height);
        m_vertices[1].position = sf::Vector2f(x + rect.width, y + rect.height);
        m_vertices[2].position = sf::Vector2f(x, y);
        m_vertices[3].position = sf::Vector2f(x + rect.width, y);
        break;
    case Right:
        m_vertices[0].position = sf::Vector2f(x + rect.width, y);
        m_vertices[1].position = sf::Vector2f(x, y);
        m_vertices[2].position = sf::Vector2f(x + rect.width, y + rect.height);
        m_vertices[3].position = sf::Vector2f(x, y + rect.height);
        break;
    }
    m_direction = direction;
}

} // namespace
