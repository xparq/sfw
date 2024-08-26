#include "sfw/layout/HBox.hpp"
#include "sfw/Theme.hpp"

#include "sfw/math/Vector.hpp"

#include <algorithm>
    using std::max;


namespace sfw
{


void HBox::recomputeGeometry()
{
	fVec2 pos{}, size{};
	foreach([&](Widget* w) {
		// Place widgets one after the other in a horizontal line:
		w->setPosition(pos);
		pos += {w->getSize().x() + Theme::MARGIN, 0.f};

		// Adjust height to the highest widget:
		auto dy = w->getSize().y();
		if (dy > size.y()) size.y(dy);
	});

	// Set layout width past the rightmost widget:
	size.x(max(0.f, pos.x() - Theme::MARGIN)); // 0 for an empty container
	Widget::setSize(size);
}


} // namespace sfw
