#include "sfw/layout/VBox.hpp"
#include "sfw/Theme.hpp"
#include "sfw/math/Vector.hpp"

#include <algorithm>
    using std::max;


namespace sfw
{

void VBox::recomputeGeometry()
{
    fVec2 pos, size;
    foreach([&](Widget* w) {
        w->setPosition(pos);
        pos.y() += w->getSize().y() + Theme::MARGIN;

        // The layout width is the largest widget width
        if (w->getSize().x() > size.x())
            size.x() = w->getSize().x();
    });
    size.y() = max(0.f, pos.y() - Theme::MARGIN); // 0 for an empty container
    Widget::setSize(size);
}

} // namespace sfw
