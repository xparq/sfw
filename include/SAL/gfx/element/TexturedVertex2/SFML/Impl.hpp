//----------------------------------------------------------------------------
//!! Make it not fixed 2D (like the future Vector<Dim>)!...
//!!
//!! See Vertex2_Impl.hpp!
//----------------------------------------------------------------------------

#ifndef _O874FUIYN79845M4709MMMH8UNYGHVR687GBY_
#define _O874FUIYN79845M4709MMMH8UNYGHVR687GBY_


#include "SAL.hpp"
#include SAL_ADAPTER(gfx/element/ColorVertex2)

#include "SAL/gfx/element/Texture.hpp"
#include "SAL/gfx/Color.hpp"

#include <type_traits> // is_same


namespace SAL::gfx::adapter
{
namespace SFML
{
	class TexturedVertex2_Impl : public ColorVertex2_Impl
	{                      using Base = ColorVertex2_Impl;
	protected:
		using native_type = Base::native_type; //! Same here!...
			static_assert(std::is_same_v<native_type, sf::Vertex>);

	public:
		// Inherited ctors...
		// Note: The missing texture coords will be initialized to zero by iVec2 itself!
		using Base::Base;
//!!		constexpr TexturedVertex2_Impl()
//!!			: _d({0,0}, Color::White, {0,0}) {}

		constexpr TexturedVertex2_Impl(fVec2 pos, Color color, iVec2 txpos)
//!!??OR:	constexpr TexturedVertex2_Impl(const fVec2& pos, Color color, const iVec2& txpos)
			: Base(pos, color) // fVec2 auto-converts to sf::Vector
			{ _d.texCoords = {(float)txpos.x(), (float)txpos.y()}; } //!! Alas, double init: overriding the default {0,0}!... :-/
				//! This is actually a float vector in SFML; quoting from Vertex.hpp:
				/// Note: although texture coordinates are supposed to be an integer
				/// amount of pixels, their type is float because of some buggy graphic
				/// drivers that are not able to process integer coordinates correctly.

		// Copy
		constexpr TexturedVertex2_Impl(TexturedVertex2_Impl&&) = default;
		constexpr TexturedVertex2_Impl(const TexturedVertex2_Impl&) = default;
		constexpr TexturedVertex2_Impl& operator = (const TexturedVertex2_Impl&) = default;
		constexpr TexturedVertex2_Impl& operator = (TexturedVertex2_Impl&&) = default;
		//!!?? Are the rval versions useful at all? Can they spare an extra copy? Wouldn't bet on it.

		// Copy from SFML
//!!?? _d not found:	constexpr TexturedVertex2_Impl(const native_type& nv) : Base::_d(nv) {}
		constexpr TexturedVertex2_Impl& operator = (const native_type& nv) { Base::_d = nv; return *this; }

		// Convert to SFML
		constexpr operator const native_type& () const { return native(); }
		constexpr operator       native_type& ()       { return native(); }


		// Accessors for texture pos...
		constexpr iVec2     _texture_position() const      { return iVec2((int)native().texCoords.x, (int)native().texCoords.y); } //! See the int<->float comment at the ctors!
//!! Type mismatch (SAL vs. native), but also confusing syntax: .prop() = x...:
//!!		constexpr iVec2&    _texture_position()            { return iVec2((int)native().texCoords.x, (int)native().texCoords.y); } //! See the int<->float comment at the ctors!

		constexpr void      _texture_position(iVec2 txpos) { native().texCoords = {(float)txpos.x(), (float)txpos.y()}; } //! See the int<->float comment at the ctors!


		//!! Be a bit more sophisticated than this embarrassment! :)
		static void _draw_trianglestrip(const gfx::RenderContext& ctx,
		                                const gfx::Texture& texture,
		                                const TexturedVertex2_Impl* v_array,
		                                unsigned v_count)
		{
			auto ctx_mod = ctx;
			ctx_mod.props.texture = &texture;
			Base::_draw_trianglestrip (ctx_mod, (const Base*)v_array, v_count);
		}

	private:
		constexpr const native_type& native() const { return _d; }
		constexpr       native_type& native()       { return _d; }
	};

} // namespace SFML


	// Save the specific adapter type to a backend-agnostic symbol...
	//!!
	//!! This looks totally useless, as this entire header is dispached by the selected backend,
	//!! so things defined here could just live directly in adapter::, no need for SFML:: too
	//!! -- but... The build system can't also dispatch the translation units yet, so it would
	//!! just compile -- and link -- everything together, causing name clashes across backends!
	//!!
	using TexturedVertex2_Impl = SFML::TexturedVertex2_Impl;


} // SAL::gfx::adapter


#endif // _O874FUIYN79845M4709MMMH8UNYGHVR687GBY_
