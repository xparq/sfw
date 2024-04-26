#include "sfw/Gfx/Elements/Box.hpp"
#include "sfw/Theme.hpp"
#include "sfw/util/diagnostics.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <cmath>

namespace sfw
{

Box::Box(Type type):
	m_type(type),
	m_activationState(ActivationState::Pressed) // Just a dummy that's different from Idle, so applyState won't bail out!
{
	applyState(ActivationState::Idle);
}

// Geometry --------------------------------------------------------------------

const sf::Vector2f& Box::getPosition() const
{
	return m_vertices[TOP_LEFT].position;
}


void Box::setPosition(float x, float y)
{
	// OpenGL will render things kinda funny otherwise:
	x = roundf(x);
	y = roundf(y);

	sf::Vector2f diff = sf::Vector2f(x, y) - getPosition();
	for (size_t i = 0; i < VERTEX_COUNT; ++i)
		m_vertices[i].position += diff;
}


void Box::setSize(float width, float height)
{
	if (width <= 0 || height <= 0)
		return;

	// OpenGL will render things kinda funny otherwise:
	width = roundf(width);
	height = roundf(height);

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


void Box::setFillColor(sf::Color color)
{
	m_fillColor = color;
}


void Box::press()
{
	applyState(ActivationState::Pressed);
}

void Box::release()
{
	applyState(ActivationState::Default);
}


bool Box::contains(float x, float y) const
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

void Box::applyState(ActivationState state)
{
	if (state == m_activationState || (state == ActivationState::Hovered && m_activationState == ActivationState::Focused))
		return;

	sf::FloatRect subrect = (sf::FloatRect)Theme::getTextureRect(m_type, state);
	float x = subrect.left;
	float y = subrect.top;
	float width = (float)Theme::borderSize;
	float height = (float)Theme::borderSize;
	setSegmentTextureCoords(TOP_SEGMENT,    x, y, width, height);
	setSegmentTextureCoords(MIDDLE_SEGMENT, x, y + height, width, height);
	setSegmentTextureCoords(BOTTOM_SEGMENT, x, y + height * 2, width, height);

	if (m_activationState == ActivationState::Pressed) // Any state change happens to cancel the "Pressed" state!
	{
		onRelease();
	}
	else if (state == ActivationState::Pressed)
	{
		onPress();
	}
	m_activationState = state;
}

void Box::draw(const gfx::RenderContext& ctx) const //override
{
	auto lstates = ctx.props;
	lstates.texture = &Theme::getTexture();
	ctx.target.draw(m_vertices, VERTEX_COUNT, sf::PrimitiveType::TriangleStrip, lstates);
	// Overdraw with a filled rect (presumably with some alpha!) if fillColor was set:
	if (m_fillColor)
	{
/*
		sf::RectangleShape r(sf::Vector2f(getSize().x - 2 * (float)Theme::borderSize,
		                                  getSize().y - 2 * (float)Theme::borderSize));
		r.setPosition(getPosition()); r.move({(float)Theme::borderSize, (float)Theme::borderSize});
*/
		sf::RectangleShape r(sf::Vector2f(getSize().x, getSize().y));
		r.setPosition(getPosition());
		r.setFillColor(m_fillColor.value());
		r.setOutlineThickness(0);
		ctx.target.draw(r, lstates);
	}
}


void Box::centerTextHorizontally(sf::Text& text)
{
	sf::Vector2f size = getSize();
	sf::FloatRect textSize = text.getLocalBounds();
	float x = roundf(getPosition().x + (size.x - textSize.width) / 2);
	text.setPosition({x, Theme::borderSize + Theme::PADDING});
}


void Box::centerVerticalTextVertically(sf::Text& text)
{
	sf::Vector2f size = getSize();
	sf::FloatRect textSize = text.getLocalBounds();
	float y = roundf(getPosition().y + (size.y - textSize.width) / 2);
	text.setPosition({Theme::getBoxHeight() - Theme::PADDING, y});
}

} // namespace sfw
