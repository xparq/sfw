#include "sfw/Widgets/DrawHost.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace sfw
{

DrawHost::DrawHost(const DrawHook& hook)
{
    setDrawHook(hook);
}


Widget* DrawHost::setDrawHook(const DrawHook& hook)
{
    m_drawHook = hook;
    return this;
}


void DrawHost::draw(const gfx::RenderContext& ctx) const
{
    m_drawHook(const_cast<DrawHost*>(this), ctx);
}

} // namespace
