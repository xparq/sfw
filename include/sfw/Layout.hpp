#ifndef GUI_LAYOUT_HPP
#define GUI_LAYOUT_HPP

#include "sfw/Widget.hpp"

#include <string>
#include <type_traits>
#include <utility>
#include <functional>

namespace sfw
{

/**
 * Base class for layouts. Layouts are special widgets which act as containers
 * See Form, HBox and VBox
 */
class Layout: public Widget
{
public:
    Layout();
    ~Layout();

    /**
     * Create new widget (from a temporary template instance) and append it
     * to the container
     * The optional `name` param. facilitates widget lookup by mnemonic.
     * @return Pointer to the new widget
     */
    template <class W> requires std::is_base_of_v<Widget, W>
    W* add(W&& tmp_widget, const std::string& name = "")
        { return (W*)(Widget*)add((Widget*)new W(std::move(tmp_widget)), name); }

    /**
     * Attach already existing widget created by the client (with new) to the container
     * The container will take care of deleting it.
     * The optional `name` param. facilitates widget lookup by mnemonic.
     * @return Pointer to the attached widget
     */
    Widget* add(Widget* widget, const std::string& name = "");

    /**
     * Also for any Widget subclasses, without tedious casting in client code
     */
    template <class W> requires std::is_base_of_v<Widget, W>
    W* add(W* widget, const std::string& name = "")
        { return (W*) (Widget*) add((Widget*)widget, name); }

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
