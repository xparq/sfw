#include "sfw/widget/DrawHost.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace sfw
{

DrawHost::DrawHost(const DrawHook& draw_hook)
{
    setDrawHook(draw_hook);
}


Widget* DrawHost::setDrawHook(const DrawHook& draw_hook)
{
    m_drawHook = draw_hook;
    return this;
}


void DrawHost::draw(const gfx::RenderContext& ctx) const
{
    m_drawHook(const_cast<DrawHost*>(this), ctx);
}

} // namespace
