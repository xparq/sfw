#ifndef _CNJERHKDNGF6C98O423576TGB87RT9NM23745T6Y807RE5T_
#define _CNJERHKDNGF6C98O423576TGB87RT9NM23745T6Y807RE5T_

#include "sfw/gfx/Render.hpp"
#include "sfw/math/Vector.hpp"

#include <SFML/Graphics/Color.hpp> //!! Use our own adapter class!
#include <SFML/Graphics/Vertex.hpp>

namespace sfw
{

class Arrow: public gfx::Drawable
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

	void move(fVec2 delta);

	void setPosition(fVec2 pos);

	fVec2 getSize() const;

public: //! <- NOT private, because draw() may be accessed directly (statically),
        //!    rather than just polymorphically (dynamically) via a pointer!
	void draw(const gfx::RenderContext& ctx) const override;

private:
	void updateGeometry(float x, float y, Direction direction);

	sf::Vertex m_vertices[4];
	Direction m_direction;
};

} // namespace sfw

#endif // _CNJERHKDNGF6C98O423576TGB87RT9NM23745T6Y807RE5T_
