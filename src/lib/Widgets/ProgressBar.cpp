#include "sfw/Widgets/ProgressBar.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm> // min, max

namespace sfw
{


ProgressBar::ProgressBar(const Cfg& cfg/*, const Style& style*/) :
	m_cfg(cfg)
{
	setFocusable(false); // Should be inherited from StaticWidget or sg.

	// Make sure the initial value is valid:
	m_value = min(); //!!?? set(min());

	// Assume % as the default unit, but only if the default range has been kept:
	if (m_cfg.unit == "!usedefault!")
	    m_cfg.unit = ((min() == 0 && max() == 100)) ? "%" : "";

	updateGeometry();
}


ProgressBar::ProgressBar(float length, Orientation orientation, LabelPlacement labelPlacement) :
	ProgressBar({
		.length = length,
		.orientation = orientation,
		.label_placement = labelPlacement
	})
{
}


ProgressBar* ProgressBar::set(float value)
{
	if (value != m_value) // Really changing?
	{
		value = std::max(min(), value);
		value = std::min(max(), value);
		m_value = value;

		updateGeometry();
	}
	return this;
}


float ProgressBar::get() const
{
	return m_value;
}


ProgressBar* ProgressBar::setRange(float min, float max, std::string unit)
{
	m_cfg.range.min = std::min(min, max);
	m_cfg.range.max = std::max(min, max);

	m_cfg.unit = unit;

	return this;
}


void ProgressBar::draw(const gfx::RenderContext& ctx) const
{
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	ctx.target.draw(m_box, sfml_renderstates);
	sfml_renderstates.texture = &Theme::getTexture();
	ctx.target.draw(m_bar, _VERTEX_COUNT_, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
	if (m_cfg.label_placement != LabelNone)
		ctx.target.draw(m_label, sfml_renderstates);
}


void ProgressBar::onThemeChanged()
{
	updateGeometry();
}

void ProgressBar::updateGeometry()
{
	// Bar...

	if (m_cfg.orientation == Horizontal)
	{
		m_box.setSize(m_cfg.length, Theme::getBoxHeight());
	}
	else
	{
		m_box.setSize(Theme::getBoxHeight(), m_cfg.length);
		if (m_cfg.label_placement == LabelOver)
		m_label.setRotation(sf::degrees(90.f));
	}

	m_label.setString("100%"); // Dummy value for "measurements" (size heuristics) only!
	m_label.setFont(Theme::getFont());
	m_label.setFillColor(Theme::input.textColor);
	m_label.setCharacterSize((unsigned)Theme::textSize);

	// Setup the widget box...

	const float x1 = Theme::PADDING;
	const float y1 = Theme::PADDING;
	const float x2 = (m_cfg.orientation == Horizontal ? m_cfg.length : Theme::getBoxHeight()) - Theme::PADDING;
	const float y2 = (m_cfg.orientation == Horizontal ? Theme::getBoxHeight() : m_cfg.length) - Theme::PADDING;
	m_bar[TopLeft]    .position = {x1, y1};
	m_bar[BottomLeft] .position = {x1, y2};
	m_bar[TopRight]   .position = {x2, y1};
	m_bar[BottomRight].position = {x2, y2};

	sf::FloatRect rect = (sf::FloatRect)Theme::getProgressBarTextureRect();
	m_bar[TopLeft]    .texCoords = sf::Vector2f(rect.left, rect.top);
	m_bar[BottomLeft] .texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
	m_bar[TopRight]   .texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
	m_bar[BottomRight].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);

	// Extend the widget box if the label will be outside the bar...
	float labelWidth  = m_label.getLocalBounds().width;
	float labelHeight = m_label.getLocalBounds().height;
	if (m_cfg.label_placement == LabelOutside)
	{
		if (m_cfg.orientation == Horizontal)
		{
			// Place the label at the right side of the bar
			m_label.setPosition({m_cfg.length + Theme::PADDING, Theme::PADDING});
			setSize({m_cfg.length + Theme::PADDING + labelWidth, m_box.getSize().y});
		}
		else
		{
			// Put it below the bar
			setSize({m_box.getSize().x, m_cfg.length + Theme::PADDING + labelHeight});
		}
	}
	else
	{
		setSize(m_box.getSize());
	}

	// Update the indicator bar & the label...

	m_label.setString(std::to_string(int(round(m_value))) + m_cfg.unit);

	auto bar_length = val_to_barlength(m_value);
	if (m_cfg.orientation == Horizontal)
	{
		m_bar[TopRight].position.x = m_bar[BottomRight].position.x
			= m_bar[TopLeft].position.x + bar_length;
	
		if (m_cfg.label_placement == LabelOver)
		{
			m_box.centerTextHorizontally(m_label);
		}
		// No need to reposition if LabelOutside.
	}
	else
	{
		m_bar[TopLeft].position.y = m_bar[TopRight].position.y
			= m_bar[BottomLeft].position.y - bar_length;

		if (m_cfg.label_placement == LabelOver)
		{
			m_box.centerVerticalTextVertically(m_label);
		}
		else if (m_cfg.label_placement == LabelOutside)
		{
			// Refresh horizontal label pos. in case its width has changed
			float labelX = (m_box.getSize().x - m_label.getLocalBounds().width) / 2;
			m_label.setPosition({labelX, m_box.getSize().y + Theme::PADDING});
		}
	}
}


float ProgressBar::track_length() const
// Requires that the widget has been sized (i.e. updateGeometry() has been called)!
{
	return m_cfg.length - 2 * Theme::PADDING;
}


float ProgressBar::val_to_barlength(float v) const
// Convert v to the length of the filled bar
{
	float dv = v - min();
/*cerr	<< "m_value: " << m_value << " of ["<< min() <<".."<< max() <<"]"
	<< ", v: " << v
	<< ", dv: " << dv
//	<< ", step: " << step()
//	<< ", range().size(): " << range().size()
	<< endl;*/	
	assert(dv >= 0);

	return round(track_length() * dv / range().size());
}

} // namespace
