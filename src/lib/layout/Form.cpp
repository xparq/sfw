#include "sfw/layout/Form.hpp"
#include "sfw/Theme.hpp"
#include "sfw/widget/Label.hpp"

#include <string>
#include <algorithm>
	using std::max;


namespace sfw
{

Form::Form():
	m_labelColumnWidth(0.f)
{
}


Widget* Form::add(std::string_view str, Widget* widget, std::string_view widgetname_override)
{
	Label* label = new Label(str);
	return add(label, widget, widgetname_override.empty() ? str : widgetname_override);
}


Widget* Form::add(Widget* label, Widget* widget, std::string_view widgetName, std::string_view labelWidgetName)
{
	//-------------------------------------------------------------------\
	// If no widget, `label` will be used as a (free-width) separator line.
	// Since Form knows nothing about its widgets, but their sizes, this
	// requires a trick to make separator lines work: they'll still be rows
	// of two widgets, but the 1st one will be a 0-sized dummy Label, and
	// `label` will become the second one, positioned right next to the
	// dummy, i.e. at the beginning of the line.
	//
	// (Two positive side-effects: a) `widgetname_override` will then belong
	// to the label widget, not the empty one. Must use nullptr explicitly
	// for the non-widget though: add("Line...", nullptr, "name_for_line"),
	// or add(separator_widget, nullptr, "name_for_separator_widget").
	// And b) add() will return `label` as the newly added widget, so it
	// can be conveniently manipulated further, as needed.)
	//
	if (widget == nullptr)
	{
		widget = label;
		label = new Label(""); //! Still margins, paddings etc...
		label->setSize(0, 0);
	}
	else // Make sure that normal labels have non-0 size:
	{
		if (auto size = label->getSize(); size.x() == 0)
			label->setSize(1, size.y()); // Its size is very likely redunant anyway!
	}
	//-------------------------------------------------------------------/

	if (label->getSize().x() > m_labelColumnWidth)
	{
		m_labelColumnWidth = label->getSize().x();
	}
	Layout::add(label, labelWidgetName);
	Layout::add(widget, widgetName);
	return widget;
}


void Form::recomputeGeometry()
{
	if (empty()) return;

	fVec2 size{};
	m_labelColumnWidth = 0;
	const auto minLineHeight = Theme::getBoxHeight();
	auto column_gap = Theme::MARGIN;

	//---------------------------------
	// Resize container...

	// Grow height & width to accomodate content, and find the widest label, too...
	for (Widget *label = begin(), *content = next(begin());
		label != end() && content != end();
		label = next(content), content = next(label))
	{
		auto lineHeight = minLineHeight;
		// Check the "label" widget
		lineHeight = max(lineHeight, label->getSize().y());
		m_labelColumnWidth = max(m_labelColumnWidth, label->getSize().x());
		// Check the "content" widget
		lineHeight = max(lineHeight, content->getSize().y());
		size = fVec2(max(size.x(), content->getSize().x()),
		             size.y() + lineHeight + Theme::MARGIN); //!! fVec2() needed to disambiguate op= candidates... :-/
	}

	// Add the max. label width + gap to the horiz. size...
	size.x(size.x() + m_labelColumnWidth + column_gap);
	setSize(size);

	//---------------------------------
	// Position children...
	
	float y = 0;
	for (Widget *label = begin(), *content = next(begin());
		label != end() && content != end();
		label = next(content), content = next(label))
	{
		label->setPosition(0, y);

		// Special-casing separator lines:
		if (label->getSize().x() == 0) {
			content->setPosition(0, y);
			//!! Might do other things to a separator `content` (e.g. restyling)...
		} else {
			content->setPosition(m_labelColumnWidth + column_gap, y);
		}

		auto lineHeight = minLineHeight;
		lineHeight = max(lineHeight, label->getSize().y());
		lineHeight = max(lineHeight, content->getSize().y());
		y += lineHeight + Theme::MARGIN;
	}
}


} // namespace sfw
