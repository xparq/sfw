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
	m_handle(Box::Click)
{
	updateGeometry();
}


Slider* Slider::setRange(float min, float max)
{
	m_cfg.range.min = std::min(min, max);
	m_cfg.range.max = std::max(min, max);
	return this;
}


Slider* Slider::setStep(float step)
{
	if (step <= 0 || step > max() - min())
	{
		cerr << "- Warning: invalid Slider step size " << step << " ignored." << endl;
	}
	else
	{
		m_cfg.step = step;
	}
	return this;
}


Slider* Slider::set(float value)
{
	// Ensure value is in bounds
	if (value < range().min)
	{
		value = range().min;
	}
	else if (value > range().max)
	{
		value = range().max;
	}
	else
	{
		// Round value to the closest multiple of step
		float temp = floor(value + step() / 2);
		value = temp - fmod(temp, step());
	}

	if (value != m_value) // Really changed?
	{
		m_value = value;
		updateView();
		onUpdate();
	}
	return this;
}


Slider* Slider::move(float delta)
{
	set(get() + delta);
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
		m_handle.setSize(handleWidth, handleHeight);

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
		m_handle.setSize(handleHeight, handleWidth);

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
		m_handle.setPosition(offset, 0);
		m_progression[2].position.x = offset;
		m_progression[3].position.x = offset;
	}
	else
	{
		m_handle.setPosition(0, offset);
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

	float track_len, pixel_distance;
	if (m_cfg.orientation == Horizontal)
	{
		track_len = getSize().x - m_handle.getSize().x - frame_thickness.x * 2;
		pixel_distance = x - frame_thickness.x - m_handle.getSize().x / 2;
	}
	else
	{
		track_len = getSize().y - m_handle.getSize().y - frame_thickness.y * 2;
		pixel_distance = y - frame_thickness.y - m_handle.getSize().y / 2;
		pixel_distance = track_len - pixel_distance; // The default direction is "inc up" (max val at min y)!
	}
	// Cap distance to track length
	pixel_distance = std::max(pixel_distance, 0.f);
	pixel_distance = std::min(pixel_distance, track_len);

	float value = min() + (max() - min()) * pixel_distance / track_len;

	value = std::max(range().min, value);
	value = std::min(range().max, value);
	return m_cfg.invert ? range().max - value : value;
}

float Slider::sliderval_to_handledistance(float v) const
// Convert value to distance between min and the center of the handle
{
	sf::Vector2f frame_thickness = { (float)Theme::borderSize, (float)Theme::borderSize }; // No padding for sliders!

	float track_len, pixel_distance;
	if (m_cfg.orientation == Horizontal)
	{
		track_len = getSize().x - m_handle.getSize().x - frame_thickness.x * 2;
		if (m_cfg.invert) v = range().max - v;
		pixel_distance = floor(frame_thickness.x
		                       + track_len * v / (range().max - range().min));
	}
	else
	{
		track_len = getSize().y - m_handle.getSize().y - frame_thickness.y * 2;
		v = range().max - v; // The default direction is "inc up" (max val at min y)!
		if (m_cfg.invert) v = range().max - v;
		pixel_distance = floor(frame_thickness.y
		                       + track_len * v / (range().max - range().min));
	}

	return pixel_distance;
}



void Slider::draw(const gfx::RenderContext& ctx) const
{
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	ctx.target.draw(m_track, sfml_renderstates);
	ctx.target.draw(m_progression, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
	ctx.target.draw(m_handle, sfml_renderstates);
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
		set(range().min);
		break;
	case sf::Keyboard::End:
		set(range().max);
		break;
	default:
		break;
	}
}


void Slider::onMousePressed(float x, float y)
{
	if (m_cfg.jumpy_thumb_click || !m_handle.contains(x, y)) // #219: Don't reposition on clicking the thumb
		set(mousepos_to_sliderval(x, y));

	m_handle.press();
}


void Slider::onMouseMoved(float x, float y)
{
	if (focused())
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			set(mousepos_to_sliderval(x, y));
		}
	}
	else if (m_handle.contains(x, y))
	{
		m_handle.applyState(WidgetState::Hovered);
	}
	else
	{
		m_handle.applyState(WidgetState::Default);
	}
}


void Slider::onMouseReleased(float, float)
{
	m_handle.release();
}


void Slider::onMouseWheelMoved(int delta)
{
	if (delta > 0) inc(); else dec();
}


void Slider::onStateChanged(WidgetState state)
{
	if (state == WidgetState::Focused || state == WidgetState::Default)
	{
		m_handle.applyState(state);
	}
}

void Slider::onThemeChanged()
{
	updateGeometry();
}


Slider* Slider::setCallback(std::function<void(Slider*)> callback)
{
	return (Slider*) Widget::setCallback( [callback] (Widget* w) { callback( (Slider*)w ); });
}

} // namespace
