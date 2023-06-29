#ifndef SFW_RENDER_HPP
#define SFW_RENDER_HPP

namespace sfw::gfx
{

template <typename Backend>
struct RenderContext_base
{
};

template <typename Backend>
class Drawable_base
{
protected:
    virtual void draw(const RenderContext_base<Backend>& ctx) const = 0;

//  Implementations should perform the actual drawing like:
//  void draw(const RenderContext& ctx) const
//  {
//      ctx.target.draw(my_SFML_drawable, ctx.props);
//  }
//
//  This way widget interfaces can be designed entirely backend-independently,
//  only the implementations would need to access it directly.
};


//----------------------------------------------------------------------------
template <typename Backend>
class Renderer_base
{
public:
//!!??    render(...);
};


} // namespace sfw::gfx


//----------------------------------------------------------------------------
//!!c++23 needed for this revolutionary feature:
//!!#warning Unknown graphics backend! Umm... well, only SFML is supported, so selecting that...
#include "sfw/cfg/USE_SFML"

#ifdef SFW_CFG_GFX_USE_SFML
#include "Render_sfml.hpp"
#else
#error Unknown graphics backend! Umm... only SFML is supported, #include "sfw/cfg/USE_SFML" to select it!
#endif


#endif // SFW_RENDER_HPP
