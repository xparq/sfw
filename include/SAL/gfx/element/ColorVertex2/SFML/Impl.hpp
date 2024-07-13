//!
//! SFML doesn't have a non-textured Vertex, so we just use its closest thing:
//! a textured vertex. The bloat of the unused texture coords is the price of
//! keeping zero-overhead direct interop with SFML's draw(vertex_array, ...).
//!
//!! Since this is meant to be inherited as a mixin by TexturedVertex2_Impl,
//!! and we'd also like to have the native-type data members both contiguous
//!! and also not duplicated (across the inheritance chain), we'd normally
//!! need CRTP for this (to avoid baking in some corresponding native data
//!! structure at each level). But, since SFML doesn't have a non-textured
//!! vertex, we're lucky to spare the hassle and just have a full SFML Vertex
//!! here, so TexturedVertex2_Impl can simply inherit this class directly
//!! without worrying about interop. issues with its "extended" vertex data
//!! (which it isn't, under the hood).
//!!
//!! Make it not fixed 2D (like the future Vector<Dim>)!...

#ifndef _LNXGHC24K5UYGY8EURM98DR6FMNYU9857BNY9857_
#define _LNXGHC24K5UYGY8EURM98DR6FMNYU9857BNY9857_


#include "SAL/math/Vector.hpp"
#include "SAL/gfx/Render.hpp"
//!!#include "SAL/gfx/Color.hpp" //!!Also include this, for the class interface (signatures)!

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>


namespace SAL::gfx::adapter
{
namespace SFML
{
	class ColorVertex2_Impl
	{
	protected:
		using native_type = sf::Vertex;
	
	public:
		ColorVertex2_Impl() : _d({0,0}, sf::Color::White) {} //!!SFML: not just sf::Color, but the init list ("ctor call")!

		ColorVertex2_Impl(fVec2 pos, sf::Color color) : _d(pos, color) {} // fVec2 auto-converts to sf::Vector
//!!??OR:	ColorVertex2_Impl(const fVec2& pos, sf::Color color) : _d(pos, color) {} // fVec2 auto-converts to sf::Vector
			
		// Copy
		ColorVertex2_Impl(const ColorVertex2_Impl&) = default;
		ColorVertex2_Impl& operator = (const ColorVertex2_Impl&) = default;

		// Copy from SFML
		ColorVertex2_Impl(const native_type& nv) : _d(nv) {}
		ColorVertex2_Impl& operator = (const native_type& nv) { _d = nv; return *this; }
		
		// Convert to SFML
		operator const native_type& () const { return native(); }
		operator       native_type& ()       { return native(); }

		// Accessors for the position, color, texture pos...
		constexpr fVec2     _position() const              { return native().position; } // fVec2 auto-converts from sf::Vector2f
		constexpr sf::Color _color() const                 { return native().color; }

		constexpr void      _position(fVec2 pos)           { native().position = pos; }
		constexpr void      _color(sf::Color c)            { native().color = c; }


		//!! Be a bit more sophisticated than this embarrassment! :)
		static void _draw_trianglestrip(const gfx::RenderContext& ctx,
		                                const ColorVertex2_Impl* v_array,
		                                unsigned v_count)
		{
			ctx.target.draw((native_type*)v_array, v_count, sf::PrimitiveType::TriangleStrip, ctx.props);
		}

	protected: //! Not private: meant to be mixed-in!
		native_type _d;

		const native_type& native() const { return _d; }
		      native_type& native()       { return _d; }
	};

} // namespace SFML


	// Save the specific adapter type to a backend-agnostic symbol...
	//!!
	//!! This looks totally useless, as this entire header is dispached by the selected backend,
	//!! so things defined here could just live directly in adapter::, no need for SFML:: too
	//!! -- but... The build system can't also dispatch the translation units yet, so it would
	//!! just compile -- and link -- everything together, causing name clashes across backends!
	//!!
	using ColorVertex2_Impl = SFML::ColorVertex2_Impl;


} // SAL::gfx::adapter


#endif // _LNXGHC24K5UYGY8EURM98DR6FMNYU9857BNY9857_
