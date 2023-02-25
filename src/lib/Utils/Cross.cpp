#include "sfw/Utils/Cross.hpp"
#include "sfw/Theme.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace gui
{

Cross::Cross()
{
    sf::FloatRect rect = (sf::FloatRect)Theme::getCrossTextureRect();
    m_vertices[0].texCoords = sf::Vector2f(rect.left, rect.top);
    m_vertices[1].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
    m_vertices[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
    m_vertices[3].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);

    updateGeometry(0, 0);
}


void Cross::setPosition(sf::Vector2f pos)
{
    updateGeometry(pos.x, pos.y);
}


void Cross::move(sf::Vector2f delta)
{
    for (int i = 0; i < 4; ++i)
    {
        m_vertices[i].position.x += delta.x;
        m_vertices[i].position.y += delta.y;
    }
}


void Cross::setSize(float) { }


sf::Vector2f Cross::getSize() const
{
    const sf::IntRect& rect = Theme::getCrossTextureRect();
    return sf::Vector2f((float)rect.width, (float)rect.height);
}


void Cross::setColor(const sf::Color& color)
{
    for (int i = 0; i < 4; ++i)
        m_vertices[i].color = color;
}


void Cross::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    auto lstates = states;
    lstates.texture = &Theme::getTexture();
    target.draw(m_vertices, 4, sf::PrimitiveType::TriangleStrip, lstates);
}


void Cross::updateGeometry(float x, float y)
{
    const sf::IntRect& rect = Theme::getCrossTextureRect();
    m_vertices[0].position = sf::Vector2f(x, y);
    m_vertices[1].position = sf::Vector2f(x, y + rect.height);
    m_vertices[2].position = sf::Vector2f(x + rect.width, y);
    m_vertices[3].position = sf::Vector2f(x + rect.width, y + rect.height);
}

}
