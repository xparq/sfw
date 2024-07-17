#include "sfw/widget/ProgressBar.hpp"

#include "sfw/Theme.hpp"

#include "SAL/geometry/Rectangle.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm> // min, max


namespace sfw
{
using namespace geometry;

ProgressBar::ProgressBar(const Cfg& cfg/*, const Style& style*/) :
	m_cfg(cfg),
	m_box(Box::Input) //!! #411 (Cringy legacy misnomer for setting the frame to sunken, from the default raised!)
{
	setFocusable(false); //!! Should be inherited from sg. like OutputWidget or StaticWidget...

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
		if (m_cfg.clamp) // See updateGeometry() for safeguarding against unclamped values!
		{
			value = std::max(min(), value);
			value = std::min(max(), value);
		}
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
	sfml_renderstates.texture = &Theme::getTexture(); //!! -> also: draw_trianglestrip(..., Theme::getTexture()...) below! :-/
	auto ctx_mod = ctx;
	ctx_mod.props = sfml_renderstates;
	//!! Doing things above in that roundabout way to emphasize the (in)direct SFML use!...

	m_box.draw(ctx_mod);
	gfx::TexturedVertex2::draw_trianglestrip(ctx_mod, Theme::getTexture(), m_bar, _VERTEX_COUNT_); //!! Reconcile the texture sent explicitly & ctx.props.texture!...
	if (m_cfg.label_placement != LabelNone)
		m_label.draw(ctx_mod);
}


void ProgressBar::onThemeChanged()
{
	updateGeometry();
}

void ProgressBar::updateGeometry()
{
	//-----------------------------------------------
	// Configure the visuals (once per init/reconf.)
	//-----------------------------------------------

	// Widget box...

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

	// Indicator bar...

	const float x1 = Theme::PADDING;
	const float y1 = Theme::PADDING;
	const float x2 = (m_cfg.orientation == Horizontal ? m_cfg.length : Theme::getBoxHeight()) - Theme::PADDING;
	const float y2 = (m_cfg.orientation == Horizontal ? Theme::getBoxHeight() : m_cfg.length) - Theme::PADDING;
	m_bar[TopLeft]    .position( {x1, y1} );
	m_bar[BottomLeft] .position( {x1, y2} );
	m_bar[TopRight]   .position( {x2, y1} );
	m_bar[BottomRight].position( {x2, y2} );

	auto barrect = fRect(Theme::getProgressBarTextureRect());
	m_bar[TopLeft]    .texture_position( {barrect.left()                , barrect.top()} );
	m_bar[BottomLeft] .texture_position( {barrect.left()                , barrect.top() + barrect.height()} );
	m_bar[TopRight]   .texture_position( {barrect.left() + barrect.width(), barrect.top()} );
	m_bar[BottomRight].texture_position( {barrect.left() + barrect.width(), barrect.top() + barrect.height()} );

	// Extend the widget box if the label will be outside the bar...

	m_label.set("100" + m_cfg.unit);
		// Shaky heuristics to find the maximum width the label might need.
		//!! Will certainly become incorrect with patterns later, but
		//!! it's already hopeless with arbitrary ranges (#287) now!
		//!! Anyway, the entire LabelOutside feature is kinda useless, TBH.
		//!! (Not that the other modes won't need care at all.)

	m_label.font(Theme::getFont());
	m_label.color(Theme::input.textColor);
	m_label.font_size(Theme::fontSize);

	float labelWidth  = m_label.size().x();
	float labelHeight = m_label.size().y();
	if (m_cfg.label_placement == LabelOutside)
	{
		if (m_cfg.orientation == Horizontal)
		{
			// Place the label at the right side of the bar
			m_label.position({m_cfg.length + Theme::PADDING, Theme::PADDING});
			setSize({m_cfg.length + Theme::PADDING + labelWidth, m_box.getSize().y()});
		}
		else
		{
			// Put it below the bar
			setSize({m_box.getSize().x(), m_cfg.length + Theme::PADDING + labelHeight});
		}
	}
	else
	{
		setSize(m_box.getSize());
	}

	//------------------------------------------------------------
	// Visual updates/adjustments needed after every state change
	//!! Should be a different function!
	//------------------------------------------------------------

	// Indicator bar & label...

	auto clamped_value = m_value;
	if (!m_cfg.clamp) // If not clamped by set(), we need to do its job here!...
	{
		clamped_value = std::max(min(), std::min(max(), m_value));
	}

	m_label.set(std::to_string(int(std::round(m_value))) + m_cfg.unit); //! The label should still show the original value,
	                                                                    //! that's the whole point of clamp = false!
	auto bar_length = val_to_barlength(clamped_value);
	if (m_cfg.orientation == Horizontal)
	{
		m_bar[TopRight].position().x() =
			m_bar[BottomRight].position().x() =
				m_bar[TopLeft].position().x() + bar_length;
	
		if (m_cfg.label_placement == LabelOver)
		{
			m_label.center_in(m_box.getRect());
		}
		// No need to reposition if LabelOutside.
	}
	else
	{
		m_bar[TopLeft].position().y() =
			m_bar[TopRight].position().y() =
				m_bar[BottomLeft].position().y() - bar_length;

		if (m_cfg.label_placement == LabelOver)
		{
			m_label.center_in(m_box.getRect());

		}
		else if (m_cfg.label_placement == LabelOutside)
		{
			// Refresh horizontal label pos. in case its width has changed
			m_label.position({(m_box.getSize().x() - m_label.size().x()) / 2,
			                   m_box.getSize().y() + Theme::PADDING});
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


} // namespace sfw
