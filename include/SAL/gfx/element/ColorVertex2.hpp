//----------------------------------------------------------------------------
//!! Make it not fixed 2D (like the future Vector<Dim>)!...
//----------------------------------------------------------------------------

#ifndef _XDFYM39487YF63DM8745Y7INUHUIEVBT9P8YUH54YH_
#define _XDFYM39487YF63DM8745Y7INUHUIEVBT9P8YUH54YH_


#include "SAL/math/Vector.hpp"
#include "SAL/gfx/Color.hpp"
#include "SAL/gfx/Render.hpp"

#include "SAL.hpp"
#include SAL_ADAPTER(gfx/element/ColorVertex2)


namespace SAL::gfx
{
	// This is a template only to exploit the fact that the SFML implementations for
	// colored vertices and also colored + textured vertices can use the same underlying
	// data (sf::Vertex for both), so this interface class can be used for defining
	// both ColorVertex2 and TexturedVertex2 as-is, *and* in the latter case it can
	// also serve as the natural mixin base of the combined interface + impl class for
	// TexturedVertex2.
	// When used for ColorVertex2, Impl will be ColorVertex2_Impl, and for TexturedVertex2
	// it will be TexturedVertex2_Impl -- which is also just derived from ColorVertex2_Impl!
	//
	// This slight-of-hand technique is only possible because of the shared (or at least
	// covariant) native_types, so it may not generalize for other adapter hierarchies!
	// (At least that's how I last understood it; now what I only want is to forget... :) )
	//
	template <class Impl>
	class ColorVertex2_Interface : public Impl
	{
	public:
		using Impl::Impl;

		// Accessors...
		constexpr fVec2&    position()              { return adapter()->_position_ref(); }
		constexpr Color&    color()                 { return adapter()->_color_ref(); }

		constexpr fVec2     position() const        { return adapter()->_position_copy(); }
		constexpr Color     color()    const        { return adapter()->_color_copy(); }

		constexpr void      position(fVec2 pos)     { adapter()->_position_set(pos); }
		constexpr void      color(Color c)          { adapter()->_color_set(c); }


		//!! Be a bit more sophisticated than this embarrassment! :)
		static void draw_trianglestrip(const gfx::RenderContext& ctx,
	                                       const ColorVertex2_Interface* v_array,
	                                       unsigned v_count)
			{ _draw_trianglestrip(ctx, v_array, v_count); } // Implicit conversion of v_array to Impl*!

	protected:
		constexpr       auto adapter()       { return static_cast<      Impl*>(this); }
		constexpr const auto adapter() const { return static_cast<const Impl*>(this); }
			//!!?? WTF did just `auto` here broke the const adapter()-> calls earlier, while
			//!!?? the pretty much isomorphic Vector accessors have always compiled fine?! :-o
			//!!?? Is the vector code actually incorrect (just `auto` losing `const`),
			//!!?? and I've just been lucky?!
	}; // class ColorVertex2_Interface


	//--------------------------------------------------------------------
	// The actual API ("dictionary") type:
	using ColorVertex2 = ColorVertex2_Interface<adapter::ColorVertex2_Impl>;


} // namespace SAL::gfx


#endif // _XDFYM39487YF63DM8745Y7INUHUIEVBT9P8YUH54YH_
