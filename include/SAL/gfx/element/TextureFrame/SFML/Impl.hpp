//!!WIP, TBD...
//!! Reconcile e.g. with the Rect (and Vector) and Image (widget) APIs!...

#ifndef _CNJERHKDNGF6C98O423576TGB87RT9NM23745T6Y807RE5T_
#define _CNJERHKDNGF6C98O423576TGB87RT9NM23745T6Y807RE5T_


#include "SAL.hpp"

#include "SAL/gfx/element/TexturedVertex2.hpp" //!! Adapt the code itself; it's still using sf::Vertex!
//!!??#include SAL_ADAPTER(gfx/element/TexturedVertex2) // Only the backend-facing part!
#include SAL_ADAPTER(gfx/element/Texture) // Only the backend-facing part!
#include "SAL/math/Vector.hpp"
//!!#include "SAL/geometry/Rectangle.hpp"
#include "SAL/gfx/Color.hpp"
#include "SAL/gfx/Render.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

//!!?? sf::Sprite has all the features, but rotation is done via trigonometry, so pixel-perfect
//!!?? results at right angles are less likely than with just reshuffling the vertices!
//!!??#include <SFML/Graphics/Sprite.hpp>

//!!?? #include <array>


namespace SAL::gfx::adapter
{
namespace SFML
{

class TextureFrame_Impl
{
public:
	enum Direction //!! Use this from SAL/geometry!
	{
		Up,
		Down,
		Left,
		Right
	};

	// The rectangle consists of a TriangleStrip of two triangles, defined by the path
	// following these indices in the default upright orientation:
	enum //!!??VertexIndex //!! Use this from SAL/geometry!
	{
		TopLeft,     // 0
		BottomLeft,  // 1
		TopRight,    // 2
		BottomRight  // 3
	};

//!!??	TextureFrame_Impl(sfw::geometry::fRect r) : TextureFrame_Impl(r.top, r.left, r.width, r.height) {}

	TextureFrame_Impl() = default;
	TextureFrame_Impl(int tx, int ty, int tdx, int tdy) {
		_texture_rect(tx, ty, tdx, tdy); _rect(0, 0, 0, 0);
	}
	TextureFrame_Impl(float tx, float ty, float tdx, float tdy) :
		TextureFrame_Impl((int)tx, (int)ty, (int)tdx, (int)tdy) {}


	// Accessors...

	// Vertex coords...

	constexpr float _x1()         const { return _vertex(TopLeft).position.x; }
	constexpr float _y1()         const { return _vertex(TopLeft).position.y; }
	constexpr float _x2()         const { return _vertex(BottomRight).position.x; }
	constexpr float _y2()         const { return _vertex(BottomRight).position.y; }
	constexpr float _x()          const { return _x1(); }
	constexpr float _y()          const { return _y1(); }
	//! These are not well-defined if the shape is no longer rectangular:
	constexpr float _dx()         const { return _vertex(BottomRight).position.x - _vertex(TopLeft).position.x; }
	constexpr float _dy()         const { return _vertex(BottomRight).position.y - _vertex(TopLeft).position.y; }

	constexpr void _rect(float x, float y, float dx, float dy) {
		//!!?? How about rounding, like in setPosition()?!
		_vertex(TopLeft    ).position = sf::Vector2f(x,      y);
		_vertex(BottomLeft ).position = sf::Vector2f(x,      y + dy);
		_vertex(TopRight   ).position = sf::Vector2f(x + dx, y);
		_vertex(BottomRight).position = sf::Vector2f(x + dx, y + dy);
	}

	// Per-vertex positions...
	//!!?? Replace these with sg. like _vertex(i).position(...), after having a SALified Vertex, too!...:
	constexpr fVec2 _position(unsigned i) const { return _vertex(i).position; } // SAL::Vector auto-converts to/from the current backend's vector type, if any.
	//!!constexpr void _position(unsigned i, float x, float y) { if constexpr (i > 3) static_assert(false, "Invalid vertex index"); _vertex(i).position = sf::Vector2f(x, y); }
	constexpr void  _position(unsigned i, float x, float y) { _vertex(i).position = sf::Vector2f(x, y); }
//!!??	constexpr void  _position(unsigned i, fVec2 pos)        { _position(i, pos.x(), pos.y()); } //!! This unpacking + repacking (for tighter DRY) might throw off the optimizer!
	constexpr void  _position(unsigned i, fVec2 pos)        { _vertex(i).position = pos; } // SAL::Vector auto-converts to/from the current backend's vector type, if any.


	// Texel coords...

	constexpr int _texture_x1() const { return (int)_vertex(TopLeft).texCoords.x; }
	constexpr int _texture_y1() const { return (int)_vertex(TopLeft).texCoords.y; }
	constexpr int _texture_x2() const { return (int)_vertex(BottomRight).texCoords.x; }
	constexpr int _texture_y2() const { return (int)_vertex(BottomRight).texCoords.y; }
	constexpr int _texture_x()  const { return _texture_x1(); }
	constexpr int _texture_y()  const { return _texture_y1(); }
	//! These are not well-defined if the shape is no longer rectangular:
	constexpr int _texture_dx() const { return (int)_vertex(BottomRight).texCoords.x - (int)_vertex(TopLeft).texCoords.x; }
	constexpr int _texture_dy() const { return (int)_vertex(BottomRight).texCoords.y - (int)_vertex(TopLeft).texCoords.y; }

	constexpr void _texture_rect(int x, int y, int dx, int dy) {
		//!!?? How about rounding, like in setPosition()?!
		_vertex(TopLeft    ).texCoords = sf::Vector2f((float)x,      (float)y);
		_vertex(BottomLeft ).texCoords = sf::Vector2f((float)x,      (float)y + dy);
		_vertex(TopRight   ).texCoords = sf::Vector2f((float)x + dx, (float)y);
		_vertex(BottomRight).texCoords = sf::Vector2f((float)x + dx, (float)y + dy);
	}


	//void rotate(float angle); //!! Only with a higher-level SFML object, that is already sf::Transformable
	//void rotate(Direction dir);

	//!! RENAME, OR REDESIGN THE API for this combined op.:
	void _place(float x, float y, Direction dir) {
		auto height = (float)_texture_dx();
		auto width  = (float)_texture_dy();
		switch (dir)
		{
		case Up:
			_vertex(0).position = sf::Vector2f(x,         y);
			_vertex(1).position = sf::Vector2f(x,         y + height);
			_vertex(2).position = sf::Vector2f(x + width, y);
			_vertex(3).position = sf::Vector2f(x + width, y + height);
			break;
		case Down:
			_vertex(0).position = sf::Vector2f(x + width, y + height);
			_vertex(1).position = sf::Vector2f(x + width, y);
			_vertex(2).position = sf::Vector2f(x,         y + height);
			_vertex(3).position = sf::Vector2f(x,         y);
			break;
		case Left:
			_vertex(0).position = sf::Vector2f(x,         y + height);
			_vertex(1).position = sf::Vector2f(x + width, y + height);
			_vertex(2).position = sf::Vector2f(x,         y);
			_vertex(3).position = sf::Vector2f(x + width, y);
			break;
		case Right:
			_vertex(0).position = sf::Vector2f(x + width, y);
			_vertex(1).position = sf::Vector2f(x,         y);
			_vertex(2).position = sf::Vector2f(x + width, y + height);
			_vertex(3).position = sf::Vector2f(x,         y + height);
			break;
		}
	}

	//! Not being a gfx::Drawable; this should be called manually:
	void _draw(const gfx::RenderContext& ctx, const gfx::Texture& texture) const
	{
		sf::RenderTarget& target = ctx.target;
		sf::RenderStates rst = ctx.props;
		rst.texture = &texture;
		target.draw(_vertices, _vertex_count, sf::PrimitiveType::TriangleStrip, rst);
	}


	// Internals...

	constexpr static inline auto _vertex_count = 4;

protected:
	//!! These beg for a Vertex abstraction:
	constexpr const sf::Vertex& _vertex(unsigned i) const { return _vertices[i]; } //!! Add DEBUG range-check!
	constexpr       sf::Vertex& _vertex(unsigned i)       { return _vertices[i]; } //!! Add DEBUG range-check!

	// Data...
	sf::Vertex _vertices[_vertex_count]; //!!?? Or maybe sf::VertexBuffer, as it lives on the GPU! (That's still not sf::Tramsformable though.)

}; // class TextureFrame_Impl

} // namespace SFML


// Save this Impl type to a backend-independent symbol:
using TextureFrame_Impl = SFML::TextureFrame_Impl; //!! Use a "standard" SAL macro for the indirectly name the current backend!


} // namespace SAL::gfx::adapter


#endif // _CNJERHKDNGF6C98O423576TGB87RT9NM23745T6Y807RE5T_
