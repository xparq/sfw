#include "sfw/layout/HBox.hpp"
#include "sfw/Theme.hpp"

#include "sfw/math/Vector.hpp"

#include <algorithm>
    using std::max;


namespace sfw
{


void HBox::recomputeGeometry()
{
    fVec2 pos, size;
    foreach([&](Widget* w) {
        w->setPosition(pos);
        pos.x() += w->getSize().x() + Theme::MARGIN;

        // The layout height is the largest widget height
        if (w->getSize().y() > size.y())
            size.y() = w->getSize().y();
    });
    size.x() = max(0.f, pos.x() - Theme::MARGIN); // 0 for an empty container
    Widget::setSize(size);
}


} // namespace sfw
