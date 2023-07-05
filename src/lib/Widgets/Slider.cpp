#include "sfw/Widgets/Slider.hpp"
#include "sfw/Theme.hpp"
#include "sfw/util/diagnostics.hpp"

#include <cmath>
#include <algorithm>
	using std::min, std::max;

#include <iostream>
	using std::cerr, std::endl;

/*
MODEL:

- The Slider range can be any closed interval of the [min, max].
  The current value will always be in that range (see later below).

  The set of allowed values, apart from the min/max bounds, is restricted
  to multiples of a `step` cfg. parameter away from min. IOW, the Slider has
  a value v in the range [min, max], such that

    v ∈ [min, max]
    
  and

    v = min + n*step (where n ∈ N), or v = max

  (see below why the extra v = max).

- Since step can be set (almost) freely (it just needs to be > 0 and <= the
  length of the range), it may unevenly divide the value range, leaving a
  reminder section at the max end of the track. This is explicitly allowed:
  it's the user's choice to model some "obscure" cases like that accurately
  at the expense of sacrificing symmetrical tracking.

UI:

- Interactions:
  - Inc./dec. by one step (e.g. via the arrow keys)
  - Set to min/max (e.g. via Home/End)
  - Set to any allowed value directly (e.g. via approximating a click/drag
    position)

- The default representation of the min -> max direction is from left to right
  in horizontal, and from bottom to top in vertical orientation.
  This can be inverted by a cfg. param.

- Inverting the slider's direction means flipping (only) the mapping of the
  min/max bounds to their corresponding UI endpoints.
  It *does not* mean any change in the (logic of the) operations themselves.

  - So, because inverting only concerns the UX, the arrows that are orthogonal
    to the current orientation (e.g. Left/Right for a vertical slider) should
    NOT be inverted. (This is relevant only for use_all_arrow_keys = true.)

*/

namespace sfw
{

Slider::Slider(const Cfg& cfg/*, const Style& style*/, float length) :
	m_cfg(cfg),
	m_boxLength(length),
	m_value(0),
	m_track(Box::Input),
	m_thumb(Box::Click)
{
	// Make sure the initial value is valid:
	set(range().min);

	updateGeometry();
}


Slider* Slider::setStep(float step)
{
	if (step < 0 || step > range().size())
	{
		cerr << "- Warning: invalid Slider step size " << step << " ignored." << endl;
		return this;
	}

	if (step == 0) setIntervals(track_length());
	else           m_cfg.step = step;

	return this;
}

Slider* Slider::setIntervals(float n)
{
	if (n != 0)
	{
		setStep(range().size() / n);
	}
	return this;
}

Slider* Slider::setRange(float min, float max, bool continuous)
{
	m_cfg.range.min = std::min(min, max);
	m_cfg.range.max = std::max(min, max);

	if (continuous)
		setStep(0);

	return this;
}

Slider* Slider::setRange(float min, float max, float step)
{
	setRange(min, max, false);
	return setStep(step);
}


Slider* Slider::set(float value)
// Snaps to the closest multiple of step. (Remember: step may be non-integer!)
{
	if (value != m_value) // Really changing?
	{
//cerr << "set("<< value <<")... [" << range().min << ".." << range().max << "], step: "<< step() <<endl;

		auto d = value - min();
		value = min() + floor( (d + step()/2) / step())
		        * step();
//cerr << " - set(): snapped value (before capping): "<< value <<endl;

		value = std::max(range().min, value);
		value = std::min(range().max, value);
		//!! NOTE: as FP errors accumulate, it might have grown out-of-bound,
		//!! but then it has now got reset by the capping that we have to do
		//!! anyway... However, this doesn't protect against lots of sliding
		//!! to and fro well within the interval!

		if (value != m_value) // Still looks like a change? :) (Note: floating-point errors make this hit-and-miss though!)
		{
			m_value = value;
//cerr << " - set(): m_value: "<< m_value <<endl;
			updateView(); //! This is equivalent to `if (changed()) updateView()`,
			              //! and also should be in onUpdated(), but for most widgets,
			              //! the view can change even if the value doesn't (e.g. in
				      //! onThemeChanged, or a text box may have resize etc. etc.)
				      //!! So, this should be an entire additional dimension to
				      //!! widget change control (apart from value, i.e. the "model";
				      //!! the "view" lives its own life, only _mostly_ depending on
				      //!! the value!
		}
	}
	return this;
}


Slider* Slider::move(float delta)
{
	update(get() + delta);
	return this;
}


void Slider::updateGeometry()
{
	// (Using unsigned short here (instead of e.g. just unsigned or size_t)
	// shuts up the "possible loss of data" warnings... Plain short would be
	// fine, too, but unsigned (where applicable) is cleaner conceptually.)
	unsigned short handleHeight = (unsigned short)Theme::getBoxHeight();
	unsigned short handleWidth = handleHeight / 2;
	unsigned short grooveHeight = (unsigned short)Theme::borderSize * 3;
	short grooveOffset = (short)(handleHeight - grooveHeight) / 2;

	if (m_cfg.orientation == Horizontal)
	{
		m_track.setSize(m_boxLength, grooveHeight);
		m_track.setPosition(0, grooveOffset);
		m_thumb.setSize(handleWidth, handleHeight);

		setSize(m_boxLength, handleHeight);

		for (int i = 0; i < 4; ++i)
		{
			m_progression[i].color = Theme::bgColor;
			m_progression[i].position.x = m_track.getPosition().x + Theme::borderSize;
			m_progression[i].position.y = m_track.getPosition().y + Theme::borderSize;
		}
		m_progression[1].position.y += m_track.getSize().y - Theme::borderSize * 2;
		m_progression[3].position.y += m_track.getSize().y - Theme::borderSize * 2;
	}
	else
	{
		m_track.setSize(grooveHeight, m_boxLength);
		m_track.setPosition(grooveOffset, 0);
		m_thumb.setSize(handleHeight, handleWidth);

		setSize(handleHeight, m_boxLength);

		for (int i = 0; i < 4; ++i)
		{
			m_progression[i].color = Theme::bgColor;
			m_progression[i].position.x = m_track.getPosition().x + Theme::borderSize;
			m_progression[i].position.y = m_track.getPosition().y + Theme::borderSize;
		}
		m_progression[2].position.x += m_track.getSize().x - Theme::borderSize * 2;
		m_progression[3].position.x += m_track.getSize().x - Theme::borderSize * 2;
	}

	updateView();
}

void Slider::updateView()
{
	float offset = sliderval_to_handledistance(m_value);

	if (m_cfg.orientation == Horizontal)
	{
		m_thumb.setPosition(offset, 0);
		m_progression[2].position.x = offset;
		m_progression[3].position.x = offset;
	}
	else
	{
		m_thumb.setPosition(0, offset);
		m_progression[0].position.y = offset;
		m_progression[2].position.y = offset;
	}
}

//----------------------------------------------------------------------------
float Slider::mousepos_to_sliderval(float x, float y) const
// Convert longitudinal position (x for horiz., y for vert.) to slider value
// (without setting it)
{
	sf::Vector2f frame_thickness = { (float)Theme::borderSize, (float)Theme::borderSize }; // No padding for sliders!
	float pixel_distance, track_len = track_length();

	if (m_cfg.orientation == Horizontal)
	{
		pixel_distance = x - frame_thickness.x - m_thumb.getSize().x / 2;
	}
	else
	{
		pixel_distance = y - frame_thickness.y - m_thumb.getSize().y / 2;
		pixel_distance = track_len - pixel_distance; // The default direction is "inc up" (max val at min y)!
	}
	// Cap distance to track length
	pixel_distance = std::max(pixel_distance, 0.f);
	pixel_distance = std::min(pixel_distance, track_len);

	float dv = pixel_distance / track_len * range().size();
	if (m_cfg.invert) dv = range().size() - dv;

	float value = min() + dv;
/*
cerr	<< "mouse -> value: " << value
	<< ", dv: " << dv
	<< ", [" << min() << ".." << max() << "], range-size: " << range().size()
	<< ", step: " << step()
	<< ", pixel_distance: " << pixel_distance
	<< ", track_len: " << track_len
	<< endl;
*/
	value = std::max(range().min, value);
	value = std::min(range().max, value);
	return value;
}

float Slider::sliderval_to_handledistance(float v) const
// Convert value to distance between min and the center of the handle
{
	sf::Vector2f frame_thickness = { (float)Theme::borderSize, (float)Theme::borderSize }; // No padding for sliders!

	float pixel_distance;
	float dv = v - range().min;
/*
cerr	<< "m_value: " << m_value << " of ["<< min() <<".."<< max() <<"]"
	<< ", v: " << v
	<< ", dv: " << dv
//	<< ", step: " << step()
//	<< ", range().size(): " << range().size()
	<< endl;
*/	
	assert(dv >= 0);

	if (m_cfg.orientation == Horizontal)
	{
		if (m_cfg.invert) dv = range().size() - dv;
		assert(dv >= 0);
		pixel_distance = floor(frame_thickness.x
		                       + track_length() * dv / range().size());
	}
	else
	{
		dv = range().size() - dv; // The default direction is "inc up" (max val at min y)!
		if (m_cfg.invert) dv = range().size() - dv;
		assert(dv >= 0);
		pixel_distance = floor(frame_thickness.y
		                       + track_length() * dv / range().size());
	}

	return pixel_distance;
}


float Slider::track_length() const
{
	sf::Vector2f frame_thickness = { (float)Theme::borderSize, (float)Theme::borderSize }; // No padding for sliders!

	return m_cfg.orientation == Horizontal
		? getSize().x - m_thumb.getSize().x - frame_thickness.x * 2
		: getSize().y - m_thumb.getSize().y - frame_thickness.y * 2
	;
}


void Slider::draw(const gfx::RenderContext& ctx) const
{
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	ctx.target.draw(m_track, sfml_renderstates);
	ctx.target.draw(m_progression, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
	ctx.target.draw(m_thumb, sfml_renderstates);
}


// callbacks ------------------------------------------------------------------

void Slider::onKeyPressed(const sf::Event::KeyEvent& key)
{
	auto faster = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
	           || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);

	auto delta = faster ? 2 * step() : step(); //!!Less hamfist!...
	auto delta_redir = m_cfg.invert ? -delta : delta;
	switch (key.code)
	{
	case sf::Keyboard::Left:
		if (m_cfg.orientation == Horizontal) dec(delta_redir);
		else if (m_cfg.use_all_arrow_keys)   dec(delta);
		break;
	case sf::Keyboard::Right:
		if (m_cfg.orientation == Horizontal) inc(delta_redir);
		else if (m_cfg.use_all_arrow_keys)   inc(delta);
		break;
	case sf::Keyboard::Down:
		if (m_cfg.orientation == Vertical)   dec(delta_redir);
		else if (m_cfg.use_all_arrow_keys)   dec(delta);
		break;
	case sf::Keyboard::Up:
		if (m_cfg.orientation == Vertical)   inc(delta_redir);
		else if (m_cfg.use_all_arrow_keys)   inc(delta);
		break;
	case sf::Keyboard::Home:
		update(range().min);
		break;
	case sf::Keyboard::End:
		update(range().max);
		break;
	default:
		break;
	}
}


void Slider::onMousePressed(float x, float y)
{
	if (m_cfg.jumpy_thumb_click || !m_thumb.contains(x, y)) // #219: Don't reposition on clicking the thumb
		update(mousepos_to_sliderval(x, y));

	m_thumb.press();
}


void Slider::onMouseMoved(float x, float y)
{
	if (focused())
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			update(mousepos_to_sliderval(x, y));
			//!!TODO: Support notify_on_drag: call set instead of update if false.
			//!!      But then it must also call updated() at the end of the drag (-> onMouseReleased).
		}
	}
	else if (m_thumb.contains(x, y))
	{
		m_thumb.applyState(WidgetState::Hovered);
	}
	else
	{
		m_thumb.applyState(WidgetState::Default);
	}
}


void Slider::onMouseReleased(float, float)
{
	m_thumb.release();
	//!!TODO: Support notify_on_drag = false: do an extra updated() after dragging
	//!!      (so, it must distinguish between release after just a click (+ "minor" accidental moves?!),
	//!!      and "real" dragging, obviously -- but the "how" of it isn't obvious to me at all...),
	//!!      and also make sure the notification is not skipped due to !changed().
	//!!      (-> onMouseMoved)
}


void Slider::onMouseWheelMoved(int delta)
{
	if (m_cfg.invert) delta = -delta;
	if (delta > 0) inc(); else dec();
	//!!if (delta > 0) inc(delta * step()); else dec(delta * step());
}


void Slider::onStateChanged(WidgetState state)
{
	if (state == WidgetState::Focused || state == WidgetState::Default)
	{
		m_thumb.applyState(state);
	}
}

void Slider::onThemeChanged()
{
	updateGeometry();
}


} // namespace
