#ifndef GUI_LAYOUT_HPP
#define GUI_LAYOUT_HPP

#include "sfw/WidgetContainer.hpp"

#include <functional>

namespace sfw
{

/**
 * Base class for layouts. Layouts are special widgets which act as containers
 * See Form, HBox and VBox
 */
class Layout: public WidgetContainer
{
protected:
    void draw(const gfx::RenderContext& ctx) const override;

    // Callbacks ---------------------------------------------------------------
    void onStateChanged(WidgetState state) override;
    void onMouseMoved(float x, float y) override;
    void onMousePressed(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onMouseWheelMoved(int delta) override;
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onKeyReleased(const sf::Event::KeyEvent& key) override;
    void onTextEntered(uint32_t unicode) override;

    inline Layout* toLayout() override { return this; }

    bool focusNextWidget();
    bool focusPreviousWidget();
    // Returns true if `widget` has the focus, otherwise false
    bool focusWidget(Widget* widget);
};

} // namespace

#endif // GUI_LAYOUT_HPP
