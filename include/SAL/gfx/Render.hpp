/*****************************************************************************
!!REQUIREMENTS:

- Like the other SAL adapters, it should interoperate seamlessly with the real
  backend "renderer" (if there even is one...), "as much as possible" (TBD!).
  -> Entirely different rendering pipelines may make this infeasible though!

 *****************************************************************************/

#ifndef _DMNE98475Y687D348057NHY6378945HY6B0V45978Y9TG898_
#define _DMNE98475Y687D348057NHY6378945HY6B0V45978Y9TG898_


namespace SAL::gfx
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


} // namespace SAL::gfx


#include "SAL.hpp"
#include SAL_ADAPTER(gfx/Render)


#endif // _DMNE98475Y687D348057NHY6378945HY6B0V45978Y9TG898_
