#ifndef GUI_FORMLAYOUT_HPP
#define GUI_FORMLAYOUT_HPP

#include "sfw/Layout.hpp"

#include <string>

namespace sfw
{

/**
 * Horizontally stacked layout with a label before each widget
 */
class Form: public Layout
{
public:
    Form();


    /**
     * Add a label + a widget to the form
     * @param label: Label for the widget. (Will also become its internal name.)
     * @param widget: Pointer to the widget created by the caller (with new)
     * @param widgetname_override: Widget name to use instead of the label
     * @return Pointer to the added widget
     */
    Widget* add(const std::string& label, Widget* widget,
                const std::string& widgetname_override = "");

    /**
     * As above, but allow any (already existing) widget as "label"
     * Now the widget can't be automatically named, so...
     * @param widgetname_override allows assigning a name explicitly
     * @param labelwidgetname_override allows assigning a name also to the "label" widget
     */
    Widget* add(Widget* labelwidget, Widget* widget,
                const std::string& widgetname_override = "", const std::string& labelwidgetname_override = "");

    /*************************************************************************
     * Various convenience helper templates are also defined to spare all the
     * upcasting from Widget*, so this would work as expected:
     *
     *     add("label", new TextBox)->setText("OK")
     *
     * and to also allow "new-less" forms like:
     *
     *     add("label", W())
     *     add(LabelWidget(), W())
     *************************************************************************/
    #include "Form.tpl"

private:
    void recomputeGeometry() override;

    float m_labelWidth;
};

} // namespace

#endif // GUI_FORMLAYOUT_HPP
