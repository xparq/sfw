#ifndef GUI_FORMLAYOUT_HPP
#define GUI_FORMLAYOUT_HPP

#include "sfw/Layout.hpp"

#include <string>

namespace sfw
{

/**
 * Horizontally stacked layout with a label before each widget
 */
class FormLayout: public Layout
{
public:
    FormLayout();

    /**
     * Add a label + an already existing widget to the form
     * @param label: Label for the widget
     * @param widget: Pointer to the widget created (with new) by the caller
     * @return Pointer to the new widget
     */
    Widget* add(const sf::String& label, Widget* widget,
                const std::string& widgetname_override = "");

    /**
     * Add a label + newly created widget to the form
     * @param label: Label for the widget
     * @param widget: Temporary "template" instance the "real" widget will be created from
     * @return Pointer to the new widget
     */
    template <class W> W* add(const sf::String& label, W&& tmp_widget, const std::string& name = "")
        requires (std::is_base_of_v<Widget, W>)
        { return (W*)(Widget*)add(label, (Widget*)new W(std::move(tmp_widget)), name); }

private:
    void recomputeGeometry() override;

    float m_labelWidth;
};

} // namespace

#endif // GUI_FORMLAYOUT_HPP
