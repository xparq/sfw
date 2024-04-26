#ifndef _DMNE98475Y687D348057NHY6378945HY6B0V45978Y9TG898_
#define _DMNE98475Y687D348057NHY6378945HY6B0V45978Y9TG898_

namespace sfw::gfx //!! Inconsistent with the ns (just sfw) of other related classes! :-o :-/
{

//----------------------------------------------------------------------------
// "Mixin base" of the real RenderContext
//!! Currently just a placeholder!
template <typename Backend>
struct RenderContext_base
{
};

//----------------------------------------------------------------------------
// Abstract "mixin base" of the real Drawable base
template <typename Backend>
class Drawable_base
{
protected:
	virtual void draw(const RenderContext_base<Backend>& ctx) const = 0;

	//  Then the actual drawing could be like:
	//  public void draw(const RenderContext& ctx) const override //! Declare it public!
	//  {
	//  	my_sfw_drawable.draw(ctx); // This is why it needs to be public!
	//  	// Or, for native SFML objects:
	//  	ctx.target.draw(my_SFML_drawable, ctx.props);
	//  }
	//
	//  This way e.g. widget interfaces can be designed backend-independently,
	//  while implementation code can keep using SFML directly.
};


//----------------------------------------------------------------------------
// "Mixin base" of the real Renderer
//!! Currently just a placeholder!
template <typename Backend>
class Renderer_base
{
public:
//!!??	render(...);
};


} // namespace sfw::gfx


#include "sfw/Gfx/backend.hpp"
#include GFX_BACKEND_SPECIFIC(Render.hpp)


#endif // _DMNE98475Y687D348057NHY6378945HY6B0V45978Y9TG898_
