#ifndef SFW_RENDER_SFML_HPP
#define SFW_RENDER_SFML_HPP

#include <SFML/Graphics/Drawable.hpp>

namespace gui
{
namespace gfx
{

struct SFML //!!extend into some proper backend representation later
{
};


//----------------------------------------------------------------------------
template <>
struct RenderContext_base<SFML>
{
    sf::RenderTarget& target;
    const sf::RenderStates& props;
};

using RenderContext = RenderContext_base<SFML>;


//----------------------------------------------------------------------------
class Drawable : public Drawable_base<SFML>, public sf::Drawable
//
// NOTE: Deriving from sf:Drawable is an entirely optional convenience feature
//       in case some of these objects would be practical to send to SFML, *BY
//       THE CLIENT CODE*, for drawing directly. SFW itself *DOES NOT CARE*
//       that these objects are now also sf::Drawable!
//       So, implementing the sf::Drawable interface would do *NOTHING* for SFW!
{
friend class Layout;
protected:
    void draw([[maybe_unused]] sf::RenderTarget& target,
              [[maybe_unused]] const sf::RenderStates& states) const override {} // It's optional now...

    //!!Sigh... Must define this one, too, otherwise the one above wins, and the
    //!!one in the base will get ignored :-/
    void draw([[maybe_unused]] const gfx::RenderContext_base<gfx::SFML>& ctx) const override {}
};


//============================================================================
class Renderer : public Renderer_base<SFML>
{
public:
    void render(const Drawable& object, const RenderContext& ctx);
};


} // namespace gfx
} // namespace gui
#endif // SFW_RENDER_SFML_HPP
