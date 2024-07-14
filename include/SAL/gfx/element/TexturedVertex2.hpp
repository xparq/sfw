//----------------------------------------------------------------------------
//!! Make it not fixed 2D (like the future Vector<Dim>)!...
//----------------------------------------------------------------------------

#ifndef _TVMCVLKUFTNHG38497YT897DNYTUINHG468R7V3D5FY_
#define _TVMCVLKUFTNHG38497YT897DNYTUINHG468R7V3D5FY_


#include "SAL/gfx/element/ColorVertex2.hpp"
#include "SAL/gfx/element/Texture.hpp"

#include "SAL.hpp"
#include SAL_ADAPTER(gfx/element/TexturedVertex2)


namespace SAL::gfx
{
	//--------------------------------------------------------------------
	template <class Impl>
	class TexturedVertex2_Interface : public ColorVertex2_Interface<Impl> //! "Indirect CRTP"
	{                           using Base = ColorVertex2_Interface<Impl>;
	public:
		// Existing Vertex2 ctors...
		// Note: The missing texture coords will be initialized to zero by iVec2 itself!
		using Base::Base;

		// Accessors for texture pos...
		constexpr iVec2     texture_position() const      { return Impl::_texture_position(); }
//!! Doesn't work, but also confusing syntax: .prop() = x...:
//!!		constexpr iVec2&    texture_position()            { return Impl::_texture_position(); }

		constexpr void      texture_position(iVec2 txpos) { Impl::_texture_position(txpos); }

		//!! Be a bit more sophisticated than this embarrassment! :)
		static void draw_trianglestrip(const gfx::RenderContext& ctx,
	                                       const gfx::Texture& texture, //!! Pass via ctx, but in an SFML-independent way!
	                                       const TexturedVertex2_Interface* v_array,
	                                       unsigned v_count)
			{ Impl::_draw_trianglestrip(ctx, texture, v_array, v_count); } // Implicit conversion of v_array to Impl*!

	}; // class TexturedVertex2_Interface


	//--------------------------------------------------------------------
	// The actual API ("dictionary") type:
	using TexturedVertex2 = TexturedVertex2_Interface<adapter::TexturedVertex2_Impl>;


} // namespace SAL::gfx


#endif // _TVMCVLKUFTNHG38497YT897DNYTUINHG468R7V3D5FY_
