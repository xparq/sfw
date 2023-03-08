#include "sfw/Layouts/VBox.hpp"
#include "sfw/Theme.hpp"

#include <algorithm>
    using std::max;

namespace sfw
{

void VBox::recomputeGeometry()
{
    sf::Vector2f pos{};
    sf::Vector2f size{};
    for (Widget* w = getFirstWidget(); w != nullptr; w = w->m_next)
    {
        w->setPosition(pos);
        pos.y += w->getSize().y + Theme::MARGIN;

        // The layout width is the largest widget width
        if (w->getSize().x > size.x)
            size.x = w->getSize().x;
    }
    size.y = max(0.f, pos.y - Theme::MARGIN); // 0 for an empty container
    Widget::setSize(size);
}

} // namespace
