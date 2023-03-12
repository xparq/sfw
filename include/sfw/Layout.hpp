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
    Layout();

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
    void onMouseLeave() override;

    inline Layout* toLayout() override { return this; }

    /**
     * Set the focus on a widget, if applicable
     * @return true if the widget took the focus, otherwise false
     */
    bool focusWidget(Widget* widget);
    bool focusNextWidget();
    bool focusPreviousWidget();

private:
    Widget* m_hover;
    Widget* m_focus;
};

} // namespace

#endif // GUI_LAYOUT_HPP
