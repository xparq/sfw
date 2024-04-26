#ifndef _OEFGUNDRYBFUNTXM7834C5T687BT8F793CN3M57_
#define _OEFGUNDRYBFUNTXM7834C5T687BT8F793CN3M57_

#include "sfw/ActivationState.hpp"
#include "sfw/Gfx/Render.hpp"

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>

#include <cmath>
#include <optional>

namespace sfw
{

/**
 * Event-sensitive textured/colored rectangle for building widgets
 */
class Box: public gfx::Drawable
{
public:
	enum Type
	{
		Input,
		Click
	};

	Box(Type type = Click);

	const sf::Vector2f& getPosition() const;

	void setPosition(float x, float y);

	void setSize(float width, float height);
	void setSize(const sf::Vector2f& size) { return setSize(size.x, size.y); }

	sf::Vector2f getSize() const;

	// Fill the interior of the box with a color, overriding its texture
	// The edges will NOT be affected!
	void setFillColor(sf::Color color);

	void press();
	void release();

	void applyState(ActivationState state);

	// Check if a point is inside the box (including its edges)
	bool contains(float x, float y) const;

	template <class T>
	void centerItem(T& item)
	{
		sf::Vector2f size = getSize();
		sf::Vector2f itemSize = item.getSize();
		// Center item
		item.setPosition(
			{roundf(getPosition().x + (size.x - itemSize.x) / 2),
			 roundf(getPosition().y + (size.y - itemSize.y) / 2)}
		);
	}

	void centerTextHorizontally(sf::Text& text);
	void centerVerticalTextVertically(sf::Text& text);

public: //! <- NOT private, because draw() may be accessed directly (statically),
        //!    rather than just polymorphically (dynamically) via a pointer!
	void draw(const gfx::RenderContext& ctx) const override;

protected:
	virtual void onPress() {};
	virtual void onRelease() {};

	Type m_type;

private:

	// A Box is drawn using a triangle strip (sf::TriangleStrip) that
	// consists of 3 isomorphic rows (segments) of 9 vertices each:
	// 8 to draw the (textured) rectangles, + 1 more for "carriage return",
	// driving the triangle strip back to the left of the next segment
	// (via 2 degenerate triangles: 6-7-8, 7-8-9):
	//
	// 0 2 4 6                                  (6)
	// |/|/|/|  ->  then 8 := 7  ->              |
	// 1 3 5 7                       (1)-(3)-(5)-8
	//
	// Then 9 goes back to 1, and the process repeats with the 2nd row etc.
	// Note that the next segment will start by repeating this last vertex,
	// resulting in yet another degenerate triangle.

	static constexpr size_t VERTICES_PER_SEGMENT = 9;
	static constexpr size_t VERTEX_COUNT = 3 * VERTICES_PER_SEGMENT;

	enum StripSegment
	{
		TOP_SEGMENT,
		MIDDLE_SEGMENT,
		BOTTOM_SEGMENT,
	};

	enum Vertex
	{
		TOP_LEFT,
		BOTTOM_RIGHT = VERTICES_PER_SEGMENT * 3 - 1,
	};

	// Setup the geometry of one "row"
	void setSegmentGeometry(StripSegment n, float x0, float x2, float x4, float x6, float top, float bottom);

	// Set texture coords. for one "row"
	void setSegmentTextureCoords(StripSegment n, float txleft, float txtop, float txwidth, float txheight);


	ActivationState m_activationState;
	sf::Vertex m_vertices[VERTEX_COUNT];
	std::optional<sf::Color> m_fillColor;
};

} // namespace sfw

#endif // _OEFGUNDRYBFUNTXM7834C5T687BT8F793CN3M57_
