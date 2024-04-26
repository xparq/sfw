#include "sfw/Widgets/Slider.hpp"
#include "sfw/Theme.hpp"
#include "sfw/util/diagnostics.hpp"

#include <cmath>
#include <algorithm> // min, max

#include <string>
	using std::to_string;

#include <cassert>

//!!Reconcile this with the diag. stuff; see #284!
#include <iostream>
	using std::cerr, std::endl;

/*
MODEL:

- The Slider range can be any closed interval of [min, max].
  The current value will always be in that range (see later below).

  The set of allowed values, apart from the min/max bounds, is restricted
  to multiples of a `step` parameter away from min. IOW, the Slider has
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

using enum ActivationState;


Slider::Slider(const Cfg& cfg/*, const Style& style = Style()*/) :
	Slider(cfg, cfg.length)
{
}

Slider::Slider(const Cfg& cfg/*, const Style& style*/, float length) :
	m_cfg(cfg),
	m_boxLength(length),
	m_value(0),
	m_track(Box::Input),
	m_thumb(Box::Click)
{
	//!
	//! The exec. order below is critical, and brittle... Everything depends
	//! on the prev. one in non-obvious ways! (-> #365)
	//!

	// Make sure the initial value is valid:
	m_value = range().min; //! updateView() requires a valid, in-range m_value, but can't call
	                       //! set(range().min), as set() depends on everything...

	updateGeometry();      //! track_length() (called by setStep) depends on the widget properly sized
	                       //! -- but can't do this first, as it calls updateView()...

	// Make sure the configured 'step' is valid (even if it won't be used later)
	// (-> #364: special treatment for 0 was skipped)
	setStep(m_cfg.step);
	                       //! Depends on UpdateGeomentry() having been called already.
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
	//!!----------------------------------------------------------------------------
	//!! THIS IS NOW DONE, BUT LEAVING IT STILL HERE AS A MEMENTO...
	//!!----------------------------------------------------------------------------
	//!!
	//!! Disabling this if(...) until clarifying the "enigma" in #317! :)
	//!! ...And until finding a clean way to get this back, while also ensuring
	//!! the an initial set() would count as a real change() (no matter what
	//!! a C++ default-inited value would be! -> Universally, for each widget!
	//!! The point here is actually the side-effects, though, so perhaps those
	//!! should be taken care of during init (construction) independently from
	//!! set()? But that'd be yet another explicit widget init chore then... :-/
	//!!
	//!!----------------------------------------------------------------------------
	//!! (The -- trivial, in hindsight... also, kinda ingenious :) -- solution was
	//!! just to init m_changed with true, instead of false in InputWidget!)
	//!!----------------------------------------------------------------------------
	if (value != m_value) // Really changing?
	                      //! NOTE: No early return, as we may have things to do even if this is false!
	                      //!!FIX: But if never actually changed (like ctor-init to 0...),
	                      //!!     then updateView, setTooltip etc. will never be called?!
	{
//cerr << "set("<< value <<")... [" << range().min << ".." << range().max << "], step: "<< step() <<endl;

		auto d = value - min();
		value = min() + floor( (d + step()/2) / step())
		        * step();
//cerr << " - set(): snapped value (before capping): "<< value <<endl;

		// Cap it:
		value = std::max(range().min, value);
		value = std::min(range().max, value);
			//!! NOTE: as FP errors accumulate, it might have grown out-of-bound,
			//!! but then it has now got reset by the capping that we have to do
			//!! anyway. However, this doesn't protect against deteriorated stepping
			//!! accuracy due to lots of sliding to-and-fro *within* the interval!

		if (value != m_value) // Still looks like a change? :) (Note: floating-point errors make this hit-and-miss though!)
		{
			m_value = value;
			setChanged();
		}

	}

//cerr << " - set(): m_value: "<< m_value <<endl;
	if (changed()) //! Note: this will be true initially even if the values are the same,
	               //! because InputWidgets are constructed with changed = true!...
	{
		updateView();	//! This could be in onUpdated() -- but for most widgets,
				//! the view can change even if the value doesn't (e.g. in
				//! onThemeChanged, or a widget may be resizable etc. etc.)
				//!! So, this should be an entire additional dimension to
				//!! widget change control (apart from value, i.e. the "model";
				//!! the "view" lives its own life, only _mostly_ depending on
				//!! the value!
		setTooltip(to_string(m_value));
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
// Requires that the widget has been sized (i.e. updateGeometry() has been called)!
{
	//!! This is not a correct check though! (No general (explicit...)
	//!! requirement that widgets must have nonzero size...)
	//!!assert(getSize().x && getSize().y);

	sf::Vector2f frame_thickness = { (float)Theme::borderSize, (float)Theme::borderSize }; // No padding for sliders!

	return m_cfg.orientation == Horizontal
		? getSize().x - m_thumb.getSize().x - frame_thickness.x * 2
		: getSize().y - m_thumb.getSize().y - frame_thickness.y * 2
	;
}


void Slider::draw(const gfx::RenderContext& ctx) const
{
	auto lctx = ctx;
	lctx.props.transform *= getTransform();
	m_track.draw(lctx);
	lctx.target.draw(m_progression, 4, sf::PrimitiveType::TriangleStrip, lctx.props);
	m_thumb.draw(lctx);
}


// callbacks ------------------------------------------------------------------

void Slider::onKeyPressed(const sf::Event::KeyEvent& key)
{
	auto faster = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
	           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);

	auto delta = faster ? 2 * step() : step(); //!!Less hamfist!...
	auto delta_redir = m_cfg.invert ? -delta : delta;
	switch (key.code)
	{
	case sf::Keyboard::Key::Left:
		if (m_cfg.orientation == Horizontal) dec(delta_redir);
		else if (m_cfg.use_all_arrow_keys)   dec(delta);
		break;
	case sf::Keyboard::Key::Right:
		if (m_cfg.orientation == Horizontal) inc(delta_redir);
		else if (m_cfg.use_all_arrow_keys)   inc(delta);
		break;
	case sf::Keyboard::Key::Down:
		if (m_cfg.orientation == Vertical)   dec(delta_redir);
		else if (m_cfg.use_all_arrow_keys)   dec(delta);
		break;
	case sf::Keyboard::Key::Up:
		if (m_cfg.orientation == Vertical)   inc(delta_redir);
		else if (m_cfg.use_all_arrow_keys)   inc(delta);
		break;
	case sf::Keyboard::Key::Home:
		update(range().min);
		break;
	case sf::Keyboard::Key::End:
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
	m_thumb_pressed = true;
}


void Slider::onMouseMoved(float x, float y)
{
	if (focused())
	{
		if (m_thumb_pressed) //!! #182: Not `if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))`
		{
			update(mousepos_to_sliderval(x, y));
			//!!TODO: Support notify_on_drag = false: call set instead of update!
			//!!      But then it must also call updated() at the end of the drag (-> onMouseReleased).
		}
	}
	else if (m_thumb.contains(x, y))
	{
		m_thumb.applyState(ActivationState::Hovered);
	}
	else
	{
		m_thumb.applyState(ActivationState::Default);
	}
}


void Slider::onMouseReleased(float, float)
{
	m_thumb.release();
	m_thumb_pressed = false;

	//!!TODO: Support notify_on_drag = false: do an extra updated() after dragging
	//!!      (so, it must distinguish between release after just a click (+ "minor" accidental moves?!),
	//!!      and "real" dragging, obviously -- but the "how" of it isn't that obvious...),
	//!!      and also make sure the notification is not skipped due to !changed().
	//!!      (-> onMouseMoved)
}


void Slider::onMouseWheelMoved(int delta)
{
	if (m_cfg.invert) delta = -delta;
	if (delta > 0) inc(); else dec();
	//!!if (delta > 0) inc(delta * step()); else dec(delta * step());
}


void Slider::onActivationChanged(ActivationState state)
{
	if (state == Focused || state == Default)
	{
		m_thumb.applyState(state);
	}
}

void Slider::onThemeChanged()
{
	updateGeometry();
}


} // namespace
