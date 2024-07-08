#ifndef _O874FUIYN79845M4709MMMH8UNYGHVR687GBY_
#define _O874FUIYN79845M4709MMMH8UNYGHVR687GBY_


#include "SAL/math/Vector.hpp"
#include "SAL/gfx/element/Texture.hpp"
#include "SAL/gfx/Render.hpp"
//#include "SAL/util/cpp/macros.hpp" // SAL_CONST_THIS

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp> //!! SALIFY!


namespace SAL::gfx
{
	class TexturedVertex2_Impl
	{
	protected:
		using native_type = sf::Vertex;
	
	public:
		TexturedVertex2_Impl()
			: _d({0,0}, sf::Color::White, {0,0}) {}
//!!??:
		TexturedVertex2_Impl(const fVec2& pos, sf::Color color, const iVec2& txpos)
//!!OK:		TexturedVertex2_Impl(fVec2 pos, sf::Color color, iVec2 txpos)
			: _d(pos, // fVec2 auto-converts to sf::Vector.
			     color,
	                     {(float)txpos.x(), (float)txpos.y()}) {} //! This is also a float vector, actually!... SFML says:
					/// Note: although texture coordinates are supposed to be an intege
					/// amount of pixels, their type is float because of some buggy graphic
					/// drivers that are not able to process integer coordinates correctly.


		TexturedVertex2_Impl(sf::Vertex v)
			: _d(v) {}

		TexturedVertex2_Impl(const TexturedVertex2_Impl&) = default;
		TexturedVertex2_Impl(TexturedVertex2_Impl&&) = default;
		TexturedVertex2_Impl& operator = (const TexturedVertex2_Impl&) = default;
		TexturedVertex2_Impl& operator = (TexturedVertex2_Impl&&) = default;

		operator       native_type& ()       { return native(); }
		operator const native_type& () const { return native(); }

		fVec2 position() const         { return fVec2(native().position); }
		sf::Color color() const        { return native().color; }
		iVec2 texture_position() const { return iVec2((int)native().texCoords.x, (int)native().texCoords.y); } //! See the int<->float comment at the ctors!

		void position(fVec2 pos)           { native().position = pos; }
		void color(sf::Color c)            { native().color = c; }
		void texture_position(iVec2 txpos) { native().texCoords = {(float)txpos.x(), (float)txpos.y()}; } //! See the int<->float comment at the ctors!


	static void _draw_strip(const gfx::RenderContext& ctx,
	                        const gfx::Texture& texture,
	                        const TexturedVertex2_Impl* v_array,
	                        unsigned v_count)
	{
		sf::RenderStates rst = ctx.props;
		rst.texture = &texture;
		ctx.target.draw((native_type*)v_array, v_count, sf::PrimitiveType::TriangleStrip, rst);
	}

	private:
		native_type _d;

		const native_type& native() const { return _d; }
		      native_type& native()       { return _d; }
	};

} // SAL::gfx


#endif // _O874FUIYN79845M4709MMMH8UNYGHVR687GBY_
