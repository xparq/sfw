#ifndef GUI_WIDGETCONTAINER_HPP
#define GUI_WIDGETCONTAINER_HPP

#include "sfw/Widget.hpp"

#include <string>
#include <type_traits>
#include <utility>
#include <functional>

namespace sfw
{

class WidgetContainer: public Widget
{
public:
    WidgetContainer();
    ~WidgetContainer();

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
    Widget* addBefore(const Widget* anchor, Widget* widget, const std::string& name = "");
    Widget* addBefore(const std::string& anchor, Widget* widget, const std::string& name = "");
    Widget* addAfter(const Widget* anchor, Widget* widget, const std::string& name = "");
    Widget* addAfter(const std::string& anchor, Widget* widget, const std::string& name = "");

    /**
     * Also for any Widget subclasses, without tedious casting in client code
     */
    template <class W> requires std::is_base_of_v<Widget, W>
    W* add(W* widget, const std::string& name = "")
        { return (W*) (Widget*) add((Widget*)widget, name); }


protected:

    // Callbacks ---------------------------------------------------------------

    // Helpoers ---------------------------------------------------------------
    /**
     * Widgets that are also widget containers can iterate their children via
     * this interface. The reason this is defined here in Widget, not there, is:
     *   a) parents of Widgets & Layouts shouldn't care about the difference,
     *   b) Widget types other than Layout may have children in the future.
     * The traversal is recursive (not just a single-level iteration).
     */
//!!    virtual void iterateChildren(std::function<void(Widget*)> f);

    /**
     * Widgets that are also widget containers can iterate their children via
     * this interface. The reason this is defined here in Widget, not there, is:
     *   a) parents of Widgets & Layouts shouldn't care about the difference,
     *   b) Widget types other than Layout may have children in the future.
     * The traversal is recursive (not just a single-level iteration).
     */
//!!    virtual void traverseChildren(std::function<void(Widget*)> f);

protected:
    Widget* m_first;
    Widget* m_last;
    Widget* m_hover;
    Widget* m_focus;
};

} // namespace

#endif // GUI_WIDGETCONTAINER_HPP
