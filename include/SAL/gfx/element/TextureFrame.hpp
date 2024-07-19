//----------------------------------------------------------------------------
// Reusable vertex frame for drawing texturerectangles in 4 directions
//!! That "in 4 directions" should be delegated to a subclass called
//!! TextureFrame4dir!
//
// Differs from normal (e.g. sf::) sprites in that it doesn't store a fixed
// texture: being really just a frame, it only takes a texture arg. for draw().
// (So the same frame can be reused to draw different images.)
//----------------------------------------------------------------------------

#ifndef _DIUNLASGRT8YGSBGHR877C24N0GS87TGR8DOUXYHYNB_
#define _DIUNLASGRT8YGSBGHR877C24N0GS87TGR8DOUXYHYNB_


#include "SAL.hpp"
#include SAL_ADAPTER(gfx/element/TextureFrame)

#include "SAL/geometry/Rectangle.hpp"
#include "SAL/gfx/element/Texture.hpp"
#include "SAL/gfx/Color.hpp"
#include "SAL/gfx/Render.hpp"


namespace SAL::gfx
{
	//!!template <class Impl = adapter::TextureFrame // mixin //!! Can't template it to avoid the macro AND keep the code nicely tucked away in a .cpp! :-(
	class TextureFrame : public adapter::TextureFrame_Impl // public mixin
	{              using Impl = adapter::TextureFrame_Impl;
	public:
		using Impl::Impl; // Elevate the backend-specific ctors!

		using Direction = Impl::Direction;

		TextureFrame(fVec2 size) : TextureFrame(0.f, 0.f, size.x(), size.y()) {}

		TextureFrame(geometry::fRect texture_rect) : Impl(texture_rect.x(), texture_rect.y(), texture_rect.dx(), texture_rect.dy()) {}


	//!! These can't work without storing the orientation and calling _place(...)! :-o
	//!!	constexpr void  position(float x, float y)     { rect(x, y, _dx(), _dy()); }
	//!!	constexpr void  position(fVec2 pos)            { rect(pos.x(), pos.y(), _dx(), _dy()); }
	//!!	constexpr void  size(float dx, float dy)       { rect(_x(), _y(), dx, dy); }
		constexpr fVec2 position() const               { return {_x(), _y()}; }
		constexpr fVec2 size()     const               { return {_dx(), _dy()}; }

		constexpr void  texture_position(int x, int y) { _texture_rect(x, y, _texture_dx(), _texture_dy()); }
		constexpr void  texture_size(int dx, int dy)   { _texture_rect(_texture_x(), _texture_y(), dx, dy); }
		constexpr iVec2 texture_position() const       { return {_texture_x(),  _texture_y()}; }
		constexpr iVec2 texture_size()     const       { return {_texture_dx(), _texture_dy()}; }


		void place(fVec2 pos, Direction dir) { _place(pos.x(), pos.y(), dir); }

//		void  setFillColor(Color color);
//		Color getFillColor() const;

		void move(fVec2 delta) {
			for (unsigned i = 0; i < _vertex_count; ++i)
			{
				_position(i, _position(i) + delta);
//!! OR, later, with SALified Vertex:
//!!				_vertex(i).position() += delta;
//!! OR EVEN, FINALLY:		_vertex(i).position += delta;
			}
		}

	public:
		//! Not being a gfx::Drawable; this should be called manually:
		void draw(const gfx::RenderContext& ctx, const gfx::Texture& texture) const { _draw(ctx, texture); }

	};

} // namespace SAL::gfx


#endif // _DIUNLASGRT8YGSBGHR877C24N0GS87TGR8DOUXYHYNB_
