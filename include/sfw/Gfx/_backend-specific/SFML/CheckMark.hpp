#ifndef _YGM78SY487Y3G45YNV87M475D70YRT7YHVM579H_
#define _YGM78SY487Y3G45YNV87M475D70YRT7YHVM579H_

#include "sfw/Gfx/Render.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace sfw
{

class CheckMark: public gfx::Drawable
{
public:
	CheckMark();

	void setPosition(sf::Vector2f pos);
	void move(sf::Vector2f delta);

	void setSize(float size);
	sf::Vector2f getSize() const;
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
