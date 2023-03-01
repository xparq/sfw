#ifndef GUI_LAYOUT_HPP
#define GUI_LAYOUT_HPP

#include "sfw/Widget.hpp"

namespace sfw
{

/**
 * Base class for layouts. Layouts are special widgets which act as containers
 * See FormLayout, HBoxLayout and VBoxLayout
 */
class Layout: public Widget
{
public:
    Layout();
    ~Layout();

    /**
     * Add a new widget in the container
     * The container will take care of widget deallocation
     * @return added widget
     */
    Widget* add(Widget* widget);
    template <typename W> W* add(W* widget) { return (W*) (Widget*) add((Widget*)widget); }

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

    Widget* getFirstWidget();

private:
    // Returns true if widget took the focus, otherwise false
    bool focusWidget(Widget* widget);

    Widget* m_first;
    Widget* m_last;
    Widget* m_hover;
    Widget* m_focus;
};

} // namespace

#endif // GUI_LAYOUT_HPP
