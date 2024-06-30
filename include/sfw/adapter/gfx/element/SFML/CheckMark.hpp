#ifndef _YGM78SY487Y3G45YNV87M475D70YRT7YHVM579H_
#define _YGM78SY487Y3G45YNV87M475D70YRT7YHVM579H_

#include "sfw/gfx/Render.hpp"
#include "sfw/math/Vector.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace sfw
{

class CheckMark: public gfx::Drawable
{
public:
	CheckMark();

	void setPosition(fVec2 pos);
	void move(fVec2 delta);

	void setSize(float size);
	fVec2 getSize() const;
	void setColor(const sf::Color& color);

public: //! <- NOT private, because draw() may be accessed directly (statically),
        //!    rather than just polymorphically (dynamically) via a pointer!
	void draw(const gfx::RenderContext& ctx) const;

private:
	void updateGeometry(float x, float y);

	sf::Vertex m_vertices[4];
};

} // namespace sfw

#endif // _YGM78SY487Y3G45YNV87M475D70YRT7YHVM579H_
