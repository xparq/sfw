#include "sfw/Gfx/Shapes/Box.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cassert>

namespace sfw
{

Box::Box(Type type):
    m_type(type),
    m_state(WidgetState::Pressed)
{
    applyState(WidgetState::Default);
}

// Geometry --------------------------------------------------------------------

const sf::Vector2f& Box::getPosition() const
{
    return m_vertices[TOP_LEFT].position;
}


void Box::setPosition(float x, float y)
{
    sf::Vector2f diff = sf::Vector2f(x, y) - getPosition();
    for (size_t i = 0; i < VERTEX_COUNT; ++i)
        m_vertices[i].position += diff;
}


void Box::setSize(float width, float height)
{
    if (width <= 0 || height <= 0)
        return;

    // Move/resize each of the 9 slices
    // 0--x1--x2--x3
    // |   |   |   |
    // y1--+---+---+
    // |   |   |   |
    // y2--+---+---+
    // |   |   |   |
    // y3--+---+---+
    float x1 =   (float)Theme::borderSize;
    float x2 = width -  Theme::borderSize;
    float x3 = width;
    float y1 =   (float)Theme::borderSize;
    float y2 = height - Theme::borderSize;
    float y3 = height;
    setSegmentGeometry(TOP_SEGMENT,    0, x1, x2, x3,  0, y1);
    setSegmentGeometry(MIDDLE_SEGMENT, 0, x1, x2, x3, y1, y2);
    setSegmentGeometry(BOTTOM_SEGMENT, 0, x1, x2, x3, y2, y3);
}

sf::Vector2f Box::getSize() const
{
    // Bottom right corner - top left corner
    return m_vertices[BOTTOM_RIGHT].position - getPosition();
}


void Box::press()
{
    applyState(WidgetState::Pressed);
}

void Box::release()
{
    applyState(WidgetState::Default);
}


bool Box::containsPoint(float x, float y) const
{
    return x > m_vertices[TOP_LEFT].position.x && x < m_vertices[BOTTOM_RIGHT].position.x
        && y > m_vertices[TOP_LEFT].position.y && y < m_vertices[BOTTOM_RIGHT].position.y;
}


void Box::setSegmentGeometry(StripSegment n, float x0, float x2, float x4, float x6, float top, float bottom)
{
    size_t index = n * VERTICES_PER_SEGMENT;
    m_vertices[index++].position = sf::Vector2f(x0, top);
    m_vertices[index++].position = sf::Vector2f(x0, bottom);
    m_vertices[index++].position = sf::Vector2f(x2, top);
    m_vertices[index++].position = sf::Vector2f(x2, bottom);
    m_vertices[index++].position = sf::Vector2f(x4, top);
    m_vertices[index++].position = sf::Vector2f(x4, bottom);
    m_vertices[index++].position = sf::Vector2f(x6, top);
    m_vertices[index++].position = sf::Vector2f(x6, bottom);
    // Extra dummy vertex for "carriage-return"
    m_vertices[index].position = m_vertices[index - 1].position;
    assert(index == n * VERTICES_PER_SEGMENT + VERTICES_PER_SEGMENT - 1);
}

void Box::setSegmentTextureCoords(StripSegment n, float txleft, float txtop, float txwidth, float txheight)
{
    size_t index = n * VERTICES_PER_SEGMENT;
    for (size_t x = 0; x <= 3; ++x)
    {
        m_vertices[index++].texCoords = sf::Vector2f(txleft + txwidth * x, txtop);
        m_vertices[index++].texCoords = sf::Vector2f(txleft + txwidth * x, txtop + txheight);
    }
    // Extra dummy vertex for "carriage-return":
    m_vertices[index].texCoords = m_vertices[index - 1].texCoords;
    assert(index == n * VERTICES_PER_SEGMENT + VERTICES_PER_SEGMENT - 1);
}


// Visual properties -----------------------------------------------------------

void Box::applyState(WidgetState state)
{
    if (state == m_state || (state == WidgetState::Hovered && m_state == WidgetState::Focused))
        return;

    sf::FloatRect subrect = (sf::FloatRect)Theme::getTextureRect(m_type, state);
    float x = subrect.left;
    float y = subrect.top;
    float width = (float)Theme::borderSize;
    float height = (float)Theme::borderSize;
    setSegmentTextureCoords(TOP_SEGMENT,    x, y, width, height);
    setSegmentTextureCoords(MIDDLE_SEGMENT, x, y + height, width, height);
    setSegmentTextureCoords(BOTTOM_SEGMENT, x, y + height * 2, width, height);

    if (m_state == WidgetState::Pressed) // Any state change happens to cancel the "Pressed" state!
    {
        onRelease();
    }
    else if (state == WidgetState::Pressed)
    {
        onPress();
    }
    m_state = state;
}


void Box::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    auto lstates = states;
    lstates.texture = &Theme::getTexture();
    target.draw(m_vertices, VERTEX_COUNT, sf::PrimitiveType::TriangleStrip, lstates);
}


void Box::centerTextHorizontally(sf::Text& text)
{
    sf::Vector2f size = getSize();
    sf::FloatRect textSize = text.getLocalBounds();
    float x = getPosition().x + (size.x - textSize.width) / 2;
    text.setPosition({x, Theme::borderSize + Theme::PADDING});
}


void Box::centerTextVertically(sf::Text& text)
{
    sf::Vector2f size = getSize();
    sf::FloatRect textSize = text.getLocalBounds();
    float y = getPosition().y + (size.y - textSize.width) / 2;
    text.setPosition({Theme::getBoxHeight() - Theme::PADDING, y});
}

} // namespace
