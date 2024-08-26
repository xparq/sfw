#include "sfw/layout/VBox.hpp"
#include "sfw/Theme.hpp"

#include "sfw/math/Vector.hpp"

#include <algorithm>
    using std::max;


namespace sfw
{


void VBox::recomputeGeometry()
{
	fVec2 pos{}, size{};
	foreach([&](Widget* w) {
		// Place widgets one after the other in a vertical column:
		w->setPosition(pos);
		pos += {0.f, w->getSize().y() + Theme::MARGIN};

		// Adjust width to the widest widget:
		auto dx = w->getSize().x();
		if (dx > size.x()) size.x(dx);
	});

	// Set layout height past the bottom widget:
	size.y(max(0.f, pos.y() - Theme::MARGIN)); // 0 for an empty container
	Widget::setSize(size);
}


} // namespace sfw
