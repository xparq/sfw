#include "sfw/Layouts/Form.hpp"
#include "sfw/Theme.hpp"
#include "sfw/Widgets/Label.hpp"

#include <string>
#include <algorithm>
    using std::max;

namespace sfw
{

Form::Form():
    m_labelWidth(0.f)
{
}


Widget* Form::add(const std::string& str, Widget* widget, const std::string& widgetname_override)
{
    Label* label = new Label(str);
    return add(label, widget, widgetname_override.empty() ? (std::string)str : widgetname_override);
}


Widget* Form::add(Widget* label, Widget* widget, const std::string& widgetName, const std::string& labelWidgetName)
{
    if (label->getSize().x > m_labelWidth)
    {
        m_labelWidth = label->getSize().x;
    }
    Layout::add(label, labelWidgetName);
    Layout::add(widget, widgetName);
    return widget;
}


void Form::recomputeGeometry()
{
    if (empty()) return;

    //
    // Calc. column & widget sizes...
    //
    sf::Vector2f size{};
    m_labelWidth = 0;
    const auto minLineHeight = Theme::getBoxHeight();

    for (Widget *label = m_first, *content = m_first->m_next; //!!?? is label->m_next UB?
         label != end() && content != end();
         label = next(content), content = next(label)) //!!?? and this?! GCC didn't warn...
    {
        auto lineHeight = minLineHeight;
        // Check the "label" widget
        lineHeight = max(lineHeight, label->getSize().y);
        m_labelWidth = max(m_labelWidth, label->getSize().x);
        // Check the "content" widget
        lineHeight = max(lineHeight, content->getSize().y);
        size.x = max(size.x, content->getSize().x);

        size.y += lineHeight + Theme::MARGIN;
    }
    size.x += m_labelWidth + Theme::MARGIN;
    setSize(size);

    //
    // Position child widgets...
    //
    float y = 0;
    for (Widget *label = m_first, *content = m_first->m_next; //!!?? is label->m_next UB?
         label != end() && content != end();
         label = next(content), content = next(label))
    {
        label->setPosition(0, y);
        content->setPosition(m_labelWidth + Theme::MARGIN, y);

        auto lineHeight = minLineHeight;
        lineHeight = max(lineHeight, label->getSize().y);
        lineHeight = max(lineHeight, content->getSize().y);
        y += lineHeight + Theme::MARGIN;
    }
}


} // namespace
