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
    if (label->getSize().x > m_labelWidth)
    {
        m_labelWidth = label->getSize().x;
    }
    Layout::add(label);
    Layout::add(widget, widgetname_override.empty() ? (std::string)str : widgetname_override);
    return widget;
}


void Form::recomputeGeometry()
{
    //
    // Determine panel size...
    //
    sf::Vector2f size{};
    m_labelWidth = 0;
    size_t i = 0;
    for (Widget* widget = m_first; widget != nullptr; widget = widget->m_next)
    {
        if (i % 2 == 0)
        {
            // Left-side: label
            if (widget->getSize().x > m_labelWidth)
                m_labelWidth = widget->getSize().x;
        }
        else
        {
            // Right-side: widget
            // Row height is at least Theme::getBoxHeight()
            size.y += max(widget->getSize().y, Theme::getBoxHeight()) + Theme::MARGIN;
            if (widget->getSize().x > size.x)
                size.x = widget->getSize().x;
        }
        ++i;
    }
    size.x += m_labelWidth + Theme::MARGIN;
    size.y -= Theme::MARGIN;
    Widget::setSize(size);

    //
    // (Re)position child widgets...
    //
    sf::Vector2f pos{};
    i = 0;
    for (Widget* widget = m_first; widget != nullptr; widget = widget->m_next)
    {
        if (i % 2 == 0)
        {
            // Left-side: label
            widget->setPosition(0, pos.y);
        }
        else
        {
            // Right-side: widget
            widget->setPosition(m_labelWidth + Theme::MARGIN, pos.y);
            pos.y += max(widget->getSize().y, Theme::getBoxHeight()) + Theme::MARGIN;
        }
        ++i;
    }
}

} // namespace