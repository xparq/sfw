#ifndef _OEFGUNDRYBFUNTXM7834C5T687BT8F793CN3M57_
#define _OEFGUNDRYBFUNTXM7834C5T687BT8F793CN3M57_


#include "sfw/ActivationState.hpp"

//!!?? Should we use the SAL headers explicitly instead, here in the sfw::parts layer?!
#include "sfw/math/Vector.hpp"
#include "sfw/geometry/Rectangle.hpp"
#include "sfw/gfx/element/TexturedVertex2.hpp"
#include "sfw/gfx/element/Text.hpp"
#include "sfw/gfx/Render.hpp"
#include "sfw/gfx/Color.hpp"

#include <cmath>
#include <optional>


namespace sfw//!!::parts
{

/*****************************************************************************
   Multi-state framed/textured (and optionally tinted) rectangle for
   building widgets
 *****************************************************************************/
class Box: public SAL::gfx::Drawable
{
public:
	enum Type
	{
		Input,
		Click
	};

	Box(Type type = Click);

	fVec2 getPosition() const;

	void setPosition(float x, float y);
	void setPosition(const fVec2& pos)  { return setPosition(pos.x(), pos.y()); }

	void setSize(float width, float height);
	void setSize(const fVec2& size)     { return setSize(size.x(), size.y()); }

	fVec2 getSize() const;
	geometry::fRect getRect() const;

	// Fill the interior of the box with a color, overriding its texture
	// The edges will NOT be affected!
	void setFillColor(Color color);

	void press();
	void release();

	void applyState(sfw/*!!*/::ActivationState state);

	// Check if a point is inside the box (including its edges)
	bool contains(float x, float y) const;

	template <class T>
	void centerItem(T& item)
	{
		fVec2 size = getSize();
		fVec2 itemSize = item.getSize();
		// Center item
		item.setPosition(
			{std::round(getPosition().x() + (size.x() - itemSize.x()) / 2),
			 std::round(getPosition().y() + (size.y() - itemSize.y()) / 2)}
			//!!(getPosition() + (size - itemSize) / 2).round()
		);
	}

public: //! <- NOT private, because draw() may be accessed directly (statically),
        //!    rather than just polymorphically (dynamically) via a pointer!
	//!!void draw(const gfx::RenderContext& ctx) const override;
	void draw(const SAL::gfx::RenderContext& ctx) const override;

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

	static constexpr unsigned VERTICES_PER_SEGMENT = 9;
	static constexpr unsigned VERTEX_COUNT = 3 * VERTICES_PER_SEGMENT;

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
	void setSegmentTextureCoords(StripSegment n, int txleft, int txtop, int txwidth, int txheight);                            


	ActivationState m_activationState;
	SAL::gfx::TexturedVertex2 m_vertices[VERTEX_COUNT];
	std::optional<Color> m_fillColor;
};


} // namespace sfw//!!::parts


#endif // _OEFGUNDRYBFUNTXM7834C5T687BT8F793CN3M57_
