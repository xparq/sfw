#include "sfw/widget/Tooltip.hpp"
#include "sfw/GUI-main.hpp" // sessionTime() etc.

#include <SFML/Window/Mouse.hpp>

#include "sfw/util/diagnostics.hpp"


namespace sfw {

//----------------------------------------------------------------------------
Tooltip::Tooltip(Widget* owner, std::string text):
	m_owner(owner) //!!See comment at the decl. of m_owner!
{
	assert(owner);
//!!	setParent(owner); //!!See comment at the decl. of m_owner!

	m_box.position = {0, 0};

	setText(text);
}

//----------------------------------------------------------------------------
Tooltip::Tooltip(Widget* owner) : Tooltip(owner, "") {}


//----------------------------------------------------------------------------
bool Tooltip::armed() const { return m_state != Off; }

//----------------------------------------------------------------------------
bool Tooltip::visible() const { return m_state == Showing || m_state == Fadeout; }

//----------------------------------------------------------------------------
void Tooltip::setState(State s)
{
	m_state = s;
	if (auto gui = getMain(); gui) m_timeStateChange = gui->sessionTime();
//cerr << "-> Tooltip::setState("<<s<<")\n";
}

//----------------------------------------------------------------------------
bool Tooltip::elapsed(float time_interval)
{
	if (auto gui = getMain(); gui && armed()) // No (meaningful) timestamp when Off!
		return gui->sessionTime() - m_timeStateChange > time_interval;
	else
		return false;
}

//----------------------------------------------------------------------------
void Tooltip::setText(const std::string& text)
{
	m_text.set(text);
	m_length = text.length();

	m_text.setCharacterSize(11);
}


//----------------------------------------------------------------------------
void Tooltip::show()
{
	if (auto gui = getMain(); gui)
	{
		// Reset the colors changed by Fadeout
		m_box.colorFill = sf::Color(255, 255, 220, 224);
		m_box.colorBorder = m_box.colorFill; m_box.colorBorder *= sf::Color(160, 160, 160, 255);
		m_text.setFillColor(sf::Color(0, 0, 0, 255));
/*!!
		m_mouseLastPos = gui->getMousePosition();
//DEBUG:		setPosition(m_mouseLastPos);
		auto adjpos = m_mouseLastPos; adjpos. -= m_owner->getAbsolutePosition();
		m_mouseLastPos -= m_owner->getAbsolutePosition();//!!?? Why is this off?
cerr << "GUI.MOUSE POS:" << m_mouseLastPos.x << ", " << m_mouseLastPos.y << "\n";
cerr << " - Parent:" << m_owner->getPosition().x << ", " << m_owner->getPosition().y << "\n";
cerr << " - P. ABS:" << m_owner->getAbsolutePosition().x << ", " << m_owner->getAbsolutePosition().y << "\n";
cerr << "- Tooltip's 'relative' MOUSE POS:" << adjpos.x << ", " << adjpos.y << "\n";
//cerr << " - W. TR.:" << m_owner->getPosition().x << ", " << m_owner->getAbsolutePosition().y << "\n";
!!*/
		setState(Showing);
	}
}

//----------------------------------------------------------------------------
void Tooltip::arm(float, float)
{
// If already visible, restart from the Showing ("sustain") phase,
// cancelling a pending fadeout

	if (getMain()) // Are we actually part of a GUI? (There should be a more self-descriptive query for this!!)
	{
		// Restart from `Showing`, if already shown/fading:
		if (visible())
		{
			show();
		}
		else
		{
			initView();
			setState(Delayed);
		}
	}
}


//----------------------------------------------------------------------------
void Tooltip::dismiss()
{
	setState(visible() ? Fadeout : Off);
}


//----------------------------------------------------------------------------
void Tooltip::initView()
{
	auto gui = getMain();
	if (!gui)  //!! Should be assert()! This fn. shouldn't be called "offline"!
		return;

	auto padding  = fVec2(3, 2); //!! Just an experimental hack "for now"! :-o
	auto size     = m_text.size() + 2.f * padding; // SFML text size hackery `+ textarea.top` -- BUT IT WAS THERE ONLY FOR THE y dim.(!) --: now part of size()!

	auto offset   = fVec2(-10, 20);
//!! Or below the widget:
//!!	auto offset = fVec2(16, size.y + 3);

	auto wA = m_owner->getParent()->getAbsolutePosition(); // The owner's pos. is relative to its parent!
	auto gM = gui->getMousePosition();
	auto pos = gM - wA;
		//!! -> #312
		pos += gui->getAbsolutePosition();
//cerr << "LOCAL MOUSE [Tooltip]: " << m.x << ", " << m.y << "\n";

	pos += offset;

	setPosition(pos);

	// Fixup: to confine to the GUI rect...

	//!! The gui can actually extend beyond the window (e.g. increasing the font size
	//!! can shift stuff beyond the window edges), so we have to clip to that!

	//!!!!---------------------------------------------------------------------------------
	//!!!!   	THIS LIES!... :-/ SEEMS TO RETURN THE SAME AS getPosition()! :-ooo
	//!!!!		-> Because getAbsPos requires a parent, but a tooltip doesn't have one, currently!
	//!!!!
	auto save = m_parent; m_parent = m_owner->getParent();
	auto abspos = getAbsolutePosition(); // - gui->getAbsolutePosition(); //!!?? or +?
	m_parent = save;
	//!!!!
	//!!!!---------------------------------------------------------------------------------

//std::cerr << "abspos bottom y: " << abspos.y + size.y << ", abspos x: " << abspos.x << "\n";//gui->getSize().y << "\n";
//cerr << "- size x: " << size.x << ", y: " << size.y << "\n";
//cerr << "- gui.size.x: " << gui->getSize().x << "\n";
	if (abspos.y() + size.y() > gui->getSize().y())
	{
		setPosition({pos.x(), getPosition().y() -2 * offset.y()});
	}
	if (abspos.x() + size.x() > gui->getSize().x())
	{
		setPosition({getPosition().x() - size.x(), pos.y()});
		if (getPosition().x() < 0)
		{
			setPosition({0, pos.y()});
		}
	}

	m_box.size = size;
	m_text.setPosition({padding.x(), padding.y()});
}


//----------------------------------------------------------------------------
void Tooltip::draw(const gfx::RenderContext& ctx) const
{
	if (visible())
	{
		auto lctx = ctx;
		lctx.props.transform *= getTransform();
		
		m_box.draw(lctx);
		m_text.draw(lctx);
	}
}


//----------------------------------------------------------------------------
void Tooltip::onTick()
{
	auto gui = getMain(); 
	if (!gui)
		return;

	switch (m_state)
	{
	case Delayed:
		if (elapsed(DELAY_TIME))
			show();	
		break;
	case Showing:
		if (elapsed(SUSTAIN_MIN + (float(m_length) * READING_RATE)))
			setState(Fadeout);
		break;
	case Fadeout:
		{uint8_t DELTA = 8;
			if (m_box.colorFill.a > DELTA) {
				m_box.colorFill.a -= DELTA;
				if (m_box.colorBorder.a) m_box.colorBorder.a -= DELTA;
				m_text.setFillColor(sf::Color(0, 0, 0, m_text.getFillColor().a - DELTA));
			}
			else setState(Off);
			/*if (elapsed(FADEOUT_TIME))
				setState(Off);	
			*/
		}
		break;
	default:;
	}
}


} // namespace sfw
