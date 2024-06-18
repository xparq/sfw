#ifndef GUI_DRAWHOST_HPP
#define GUI_DRAWHOST_HPP

#include "sfw/Widget.hpp"

#include <functional>

namespace sfw
{

/**
 * Widget with a draw "hook" that can be set dynamically at runtime
 */
class DrawHost: public Widget
{
public:
    using DrawHook = std::function<void(Widget*, const gfx::RenderContext&)>;

    DrawHost(const DrawHook& drawHook);
    Widget* setDrawHook(const DrawHook& drawHook);

    // Proxying some protected members:
    Widget* setSize(const sf::Vector2f& size)  { return Widget::setSize(size); }
    Widget* setSize(float width, float height) { return Widget::setSize(width, height); }
    sf::Vector2f getPosition() const { return Widget::getAbsolutePosition(); }

private:
    void draw(const gfx::RenderContext& ctx) const override;

    DrawHook m_drawHook;
};

} // namespace

#endif // GUI_DRAWHOST_HPP
