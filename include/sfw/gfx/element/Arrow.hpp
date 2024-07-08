//----------------------------------------------------------------------------
// A simple arrow sign (for widget parts) in 4 main directions
//----------------------------------------------------------------------------

#ifndef _ODIU4WE5OYHNUDHSRGUIFYHB87ERHO78GD4N87RGT_
#define _ODIU4WE5OYHNUDHSRGUIFYHB87ERHO78GD4N87RGT_


#include "SAL/gfx/element/TextureFrame.hpp"
//!!#include "SAL/geometry/Rectangle.hpp" //!! sfw/Rect.hpp, with a simple alias sfw::Rect -> SAL::geometry::...

#include "sfw/gfx/Render.hpp" //!! SALIFY!...
#include "sfw/math/Vector.hpp"


namespace sfw//!!::gfx
{
	class Arrow : public /*!!??SAL::!!*/gfx::Drawable 
	{
	public:
		using Direction = SAL::gfx::TextureFrame::Direction;

		Arrow(Direction);

		void setFillColor(const sf::Color& color); //!! SALIFY sf::Color!

		void move(fVec2 delta);

		void setPosition(fVec2 pos);

		fVec2 getSize() const;

	public: //! <- NOT private, because draw() may be called via the direct subclass
		//!    statically, rather than always polymorphically (dynamically)!
		void draw(const /*!!??SAL::!!*/gfx::RenderContext& ctx) const override;

	protected:
		void updateGeometry(fVec2 pos, Direction direction);

		Direction m_direction;
		SAL::gfx::TextureFrame m_frame;
	};

} // namespace sfw//!!::gfx


#endif // _ODIU4WE5OYHNUDHSRGUIFYHB87ERHO78GD4N87RGT_
