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

    /**
     * Also for any Widget subclasses, without tedious casting in client code
     */
    template <class W> requires std::is_base_of_v<Widget, W>
    W* add(W* widget, const std::string& name = "")
        { return (W*) (Widget*) add((Widget*)widget, name); }

    /**
     * If the `anchor` is `nullptr`, that means "add before the first", or "add
     * after the last", respectively.
     * In case the anchor can't be found, the widget will be appended with a
     * vanilla add().
     * Note: the anchor widget isn't `const` because its next/prev pointers
     * will be updated.
     */
    Widget* addBefore(Widget* anchor, Widget* widget, const std::string& name = "");
    Widget* addBefore(const std::string& anchor_name, Widget* widget, const std::string& name = "");
    Widget* addAfter(Widget* anchor, Widget* widget, const std::string& name = "");
    Widget* addAfter(const std::string& anchor_name, Widget* widget, const std::string& name = "");

    // STL-like iteration helpers --------------------------------------------
    bool empty() const { return !m_first; }
    Widget* begin() const { return m_first; }
    Widget* end() const { return nullptr; }
    Widget* next(const Widget* w) const { return w ? w->m_next : end(); }
    Widget* prev(const Widget* w) const { return w ? w->m_previous : end(); }
    Widget* rend() const { return nullptr; }
    Widget* rbegin() const { return m_last; }
    //!! Make the entire class STL-compatible, so the std:: algorithms can be used on it! (#162)

protected:
    // Internal helpers ------------------------------------------------------
    //!!Some of them might as well be made public!
    Widget* insert_after(Widget* anchor, Widget* widget, const std::string& name);

    // Check if widget is a (direct or top-level) child of the container
    bool is_child(const Widget* widget);

    // Enumerate (only) the direct (top-level) children of the container
    //!!This may also need to be defined in Widget instead, because client
    //!!code shouldn't really care whether some widgets can or cannot have
    //!!children! A pure tree structure in this regard too could be preferable.
    void for_each_child(const std::function<void(Widget*)>& f);

    // Recursive traversal of all the contained widgets
    // (Does not include this container itself.)
    void traverse(const std::function<void(Widget*)>& f) override;

protected:
    Widget* m_first;
    Widget* m_last;
};

} // namespace

#endif // GUI_WIDGETCONTAINER_HPP
