#ifndef _D0X98457YT8567YM98CU3589VBHJUIM08B89045V90_
#define _D0X98457YT8567YM98CU3589VBHJUIM08B89045V90_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <utility> // forward, move

#ifdef DEBUG
# include <iostream>
#endif

namespace sfw::gfx
{

struct SFML //!! Just a tag yet. Extend into a proper backend adapter! (And move it out of here!)
{
};


//----------------------------------------------------------------------------
template <>
struct RenderContext_base<SFML>
{
	sf::RenderTarget& target;
	sf::RenderStates props;

	RenderContext_base(sf::RenderTarget& _target,
	                   sf::RenderStates _props = sf::RenderStates::Default)
		: target(_target)
		, props(_props)
	{}

	RenderContext_base(const RenderContext_base& other) = default;
/*		: target(other.target)
		, props(other.props)
	{
#ifdef DEBUG
std::cerr << "RenderContext_base<SFML> copy ctor\n";
#endif
	}
*/
	RenderContext_base(RenderContext_base&& other) noexcept = default;
/*
		: target(other.target) //!!?? target(std::move(other.target))
		, props(std::move(other.props))
	{
#ifdef DEBUG
std::cerr << "RenderContext_base<SFML> move ctor\n";
#endif
	}
*/
	//! Can't have these, because of the `target` ref:
	auto operator=(const RenderContext_base&) = delete;
	auto operator=(RenderContext_base&&) = delete;

//!!??	For an SFML-like draw() API, later perhaps:
//!!??	draw(sfw::Drawable...);
//!!??	draw(sf::Drawable...); //!!?? Even this ("these" then!)?...
};

using RenderContext = RenderContext_base<SFML>;


//----------------------------------------------------------------------------

using Drawable = Drawable_base<SFML>;

#if 0 // Old dual-backend ("multimorphic" :) ) experimental base:
class Drawable : public Drawable_base<SFML>, protected /*!!??public??!!*/ sf::Drawable
//
// NOTE: Deriving from sf:Drawable is an entirely optional convenience feature
//       in case some of these objects would be practical to send to SFML, *BY
//       THE CLIENT CODE*, for drawing directly. SFW itself *DOES NOT CARE*
//       that these objects are now also sf::Drawable!
//       So, implementing the sf::Drawable interface would do *NOTHING* for SFW!
{
friend class Layout;
protected:

	//
	// Override one (or both) of the draw(...) methods below!
	//

	void draw([[maybe_unused]] sf::RenderTarget& target,
	          [[maybe_unused]] sf::RenderStates states) const override {}

	//!!Sigh... Must "redefine" this here, too, otherwise the one above wins,
	//!!and the abstract one in Drawable_base will be ignored! :-/
	void draw(const RenderContext& ctx) const override { draw(ctx.target, ctx.props); } // -> sf::...::draw()
//!!??	void draw(const RenderContext& ctx) const override { ctx.target.draw((...??!!)*this, ctx.props); }
};
#endif

//============================================================================
class Renderer : public Renderer_base<SFML>
{
public:
	void render(const Drawable& object, const RenderContext& ctx);
};


} // namespace sfw::gfx

#endif // _D0X98457YT8567YM98CU3589VBHJUIM08B89045V90_
