#ifndef _YGM78SY487Y3G45YNV87M475D70YRT7YHVM579H_
#define _YGM78SY487Y3G45YNV87M475D70YRT7YHVM579H_


#include "sfw/math/Vector.hpp"
#include "sfw/gfx/Color.hpp"
#include "sfw/gfx/element/TexturedVertex2.hpp"
#include "sfw/gfx/Render.hpp"


namespace sfw//!!::parts
{

class CheckMark: public gfx::Drawable
{
public:
	CheckMark();

	void setPosition(fVec2 pos);
	void move(fVec2 delta);

	void setSize(float size);
	fVec2 getSize() const;
	void setColor(Color color);

public: //! <- NOT private, because draw() may be accessed directly (statically),
        //!    rather than just polymorphically (dynamically) via a pointer!
	void draw(const gfx::RenderContext& ctx) const;

private:
	void updateGeometry(float x, float y);

	gfx::TexturedVertex2 m_vertices[4];

}; // class CheckMark

} // namespace sfw//!!::parts


#endif // _YGM78SY487Y3G45YNV87M475D70YRT7YHVM579H_
