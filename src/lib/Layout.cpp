#include "sfw/Layout.hpp"
#include "sfw/Theme.hpp"
#include "sfw/widget/Tooltip.hpp"
#include "sfw/gfx/Render.hpp"
#include "sfw/util/adapter/sfml.hpp" // for sf::Event::KeyChanged::==

#ifdef DEBUG
#   include "sfw/GUI-main.hpp"
#   include <iostream>
	using std::cerr, std::endl;

	namespace {
		static auto DEBUG_INSIGHT_KEY = sf::Keyboard::Key::LAlt;
		static auto DEBUG_INSIGHT_KEY_PRESSED = false;
	}

#endif

namespace sfw
{

using enum ActivationState;


Layout::Layout():
	m_hoveredWidget(nullptr),
	m_focusedWidget(nullptr)
{
}

// Overrides -----------------------------------------------------------------

void Layout::draw(const gfx::RenderContext& ctx) const
{
	auto sfml_renderstates = ctx.props;
	sfml_renderstates.transform *= getTransform();
	gfx::RenderContext lctx{ctx.target, sfml_renderstates};

#ifdef DEBUG
	if (DEBUG_INSIGHT_KEY_PRESSED && getActivationState() == Hovered) {
		if (auto* root = getMain(); root) {
//cerr << getAbsolutePosition().x << ", " << getAbsolutePosition().y << endl;
			root->draw_outline(ctx, sf::Color::Yellow);
		}
		draw_outline(lctx, sf::Color::White);
	}
#endif

	cforeach([&](auto* widget) {
		if (m_hoveredWidget != widget) 	//! Defer the hovered one, for "cheating" the Z-order; see below...
			widget->draw(lctx);
#ifdef DEBUG
		// "Dim" the widget rect. if disabled:
		if (widget->disabled()) {
			widget->draw_outline(lctx, sf::Color::Transparent, Theme::bgColor * sf::Color(200, 200, 200, 50));
		}
		// Draw an outline around the widget:
		if (DEBUG_INSIGHT_KEY_PRESSED) {
			if (widget->getActivationState() == Hovered) {
				if (auto* root = getMain(); root) {
					widget->draw_outline(lctx,
						const_cast<Widget*>(widget)->isLayout() ? sf::Color::White : sf::Color::Blue);
				}
			} else if (widget->getActivationState() == Focused) {
				if (auto* root = getMain(); root) {
					widget->draw_outline(lctx,
						const_cast<Widget*>(widget)->isLayout() ? sf::Color::Cyan : sf::Color::Green);
				}
			}
		}
#endif
	});

	//! Draw the hovered item (which is often just a container) last, to win the Z-order! :)
	//!! But this z-order disturbance may be way too aggressive IRL! Test with real overlapping crap!
	if (m_hoveredWidget)
		m_hoveredWidget->draw(lctx);

	// Separate round for tooltips, to ensure they're on the top...
	//!! Alas, they can still be covered by any other layout that may be above in the Z-order!
	//!! (Note: all this here is layout-local!)
	//!! So, after all, it still must be drawn at the top ("Main") level, to ensure proper layering!
	/*!!
	if (Theme::cfg.multiTooltips)
	{
		//!!If multiple tooltips are visible and are too close to each other, and the most recent is
		//!!iterated earlier, it WILL NOT be the topmost! Proper stacking or explicit Z-ordering needed!
		//!!But, honestly... tooltips! There shouldn't even be more than one. ;)
		cforeach([&lctx](auto widget) {
			if (widget->m_tooltip)
				widget->m_tooltip->draw(lctx);
		});
	}
	else
	{
		if (m_hoveredWidget && m_hoveredWidget->m_tooltip)
			m_hoveredWidget->m_tooltip->draw(lctx);
	}
	!!*/
}


//----------------------------------------------------------------------------
void Layout::onActivationChanged(ActivationState state)
{
	if (state == Default)
	{
		// Make sure no child is stuck focused/hovered
		// NOTE: Both of these happened before this safeguard!
		//!!Which means there's a hole in the event logic somewhere... :-/
		unfocus();
		unhover();
//cerr <<"Cancelled focus/hover of child " << (m_focusedWidget->isLayout() ? " (container!) ":"") << m_focusedWidget <<endl;
	}
}


//----------------------------------------------------------------------------
void Layout::onMouseMoved(float x, float y)
{
	// Focused widgets still receive MouseMove events even when not hovered, when the mouse button is pressed
	if (m_focusedWidget && m_focusedWidget->enabled() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		m_focusedWidget->onMouseMoved(x - m_focusedWidget->getPosition().x, y - m_focusedWidget->getPosition().y);
		// "Unmark" it as the currently hovered child if wandering off of it, though:
		if (!m_focusedWidget->contains({x, y}))
		{
			m_hoveredWidget = nullptr;
		}
		return;
	}

	for (Widget* widget = begin(); widget != end(); widget = next(widget)) //!! Not a nice fit for foreachb(), as it would
	                                                                       //!! quickly become way more convoluted than this
	                                                                       //!! simple form, due to that early return & continue!
									       //!! -> #318
	{
		if (!widget->enabled())
			continue;

		// Translate mouse pos. to child-local
		sf::Vector2f localPos = sf::Vector2f(x, y) - widget->getPosition();

		if (widget->contains(localPos))
		{
			if (widget != m_hoveredWidget) // Hovered to another widget?
			{
				hover(widget, x, y);
/*!!
auto gM = getMain()->getMousePosition();
auto wA = widget->getAbsolutePosition();
auto m = gM - wA;
	//!!m += getMain()->getAbsolutePosition();
cerr << "LOCAL MOUSE [Layout]: " << m.x << ", " << m.y << endl;
//cerr << "W abs: " << wA.x << ", " << wA.y << endl;
!!*/
			}
			//else //! With this `else` still in place, some cases of #45 were still missed!
			       //! Note: hover() above could also just be recursive (like unhover() is!),
			       //! or at least call onMouseMoved internally to trigger the recursion, but
			       //! it doesn't have `localPos` handy, so best to just do that here:
			{
				widget->onMouseMoved(localPos.x, localPos.y);

				//! NOTE: If tooltip cancellation would ever be moved (back) here:
				//! Not checking for visible() would prevent tooltips from *appearing* at all,
				//! because MouseMove events typically follow a MouseEnter immediately,
				//! i.e. while the tooltip has been armed (delayed), but not yet visible!
			}
			return;

		} // if (contains)
	} // for

	// No widget is hovered now, so reset the last hovered one
	unhover();
}


//----------------------------------------------------------------------------
void Layout::onMousePressed(float x, float y)
{
	// If there's a click, it may be on the most recently hovered widget, in which case
	// we'll need to focus it.
	// However, hover processing solely via mouse moves are flaky (or used to be, see #45),
	// and sometimes (esp. with quick mouse moves across high widget boundary-line flux :) )
	// those events could be lost! So, to be sure, (re)hovering the clicked widget should
	// be done first, just in case...
	//
	// Note:
	//
	// a)  Without this, there used to be cases, where a widget did not receive the
	//     MousePressed event due to m_hoveredWidget accidentally still being 0,
	//     despite actually being hovered.
	//
	// b)  Albeit the logic below could be changed to send `MousePressed` no matter the
	//     hover state -- the target widget would then still need to be found anyway, so
	//     it's cleaner to take care of the missed hover first, and continue from there.
	//
	if (!m_hoveredWidget)
	{
		for (Widget* widget = begin(); widget != end(); widget = next(widget)) //!! Not a nice fit for foreachb() (-> #318),
		                                                                       //!! because of an early break-out!
		{
			if (widget->enabled())
			{
				// Translate mouse pos. to child-local
				sf::Vector2f localPos = sf::Vector2f(x, y) - widget->getPosition();
				if (widget->contains(localPos))
				{
//!! This is *still* necessary, even after recursifying hover (to finally
//!! propagate through a container chain down to the target widget)!... :-o
					hover(widget, x, y);
//!! (I HAVE SEEN THIS MESSAGE both in e.g. the OON control panel, and in the sfw test app!)
#ifdef DEBUG
cerr << "- [sfw::Layout::onMousePressed] Missed hover event retroactively triggered!\n";
#endif
					assert(m_hoveredWidget);
					break;
				}
			}
		}
	}

	// If clicking the hovered widget, try focusing it...
	if (m_hoveredWidget && m_hoveredWidget->enabled())
	{
		if (m_focusedWidget != m_hoveredWidget)
		{
			focus(m_hoveredWidget);
				//! It might not have taken it, but never mind, carry on regardless...
		}
		// Send event to widget
		sf::Vector2f mouse = sf::Vector2f(x, y) - m_hoveredWidget->getPosition();
		m_hoveredWidget->onMousePressed(mouse.x, mouse.y);
	}
	else if (m_focusedWidget && m_focusedWidget->enabled()) // Clicked away from the focused widget?
	{
		m_focusedWidget->setActivationState(Default); //!!->unfocus()!
		m_focusedWidget = nullptr;
	}

	// Finally, if the click is outside the managed area, give up
	// the focus, in order to support relaying events between multiple
	// indepentent GUIs (windows/panes) in an app's event loop (#362)!
	if (!contains(sf::Vector2f(x, y)))
	{
		unfocus();
	}
}


void Layout::onMouseReleased(float x, float y)
{
	// If there's a focused widget, it should see this event
	if (m_focusedWidget && m_focusedWidget->enabled())
	{
		sf::Vector2f localMousePos = sf::Vector2f(x, y) - m_focusedWidget->getPosition();
		m_focusedWidget->onMouseReleased(localMousePos.x, localMousePos.y);
		m_focusedWidget->setActivationState(Focused);
	}
}


void Layout::onMouseWheelMoved(int delta)
{
	if (m_focusedWidget)
	{
		m_focusedWidget->onMouseWheelMoved(delta);
	}
}


void Layout::onKeyPressed(const sf::Event::KeyChanged& key)
{
	//!! Handle hotkeys (with bottom-up context bubbling, somehow inverting the top-down
	//!! logic of the event flow from Main -> container(s) -> leaf widget) -> #277
	//!! Currently Tab cycling is the only thing having hotkeys, and it's hardcoded, too:
	if (key == Theme::nextWidgetKey)
	{
		if (!focusNext()) // false means no widget is focused now, and no suitable next w.
			 focusNext(); // Try to wrap around to the first widget then
		return; // Finish with this key, even if couldn't do anything with it!
	}
	else if (key == Theme::previousWidgetKey)
	{
		if (!focusPrevious()) // No suitable prev. w., so no widget is focused now
			 focusPrevious(); // Try to wrap around to the last widget then
		return; // Finish with this key, even if couldn't do anything with it!
	}

	if (m_focusedWidget)
	{
		m_focusedWidget->onKeyPressed(key);
	}

#ifdef DEBUG
	if (key.code == DEBUG_INSIGHT_KEY) DEBUG_INSIGHT_KEY_PRESSED = true;
#endif
}


void Layout::onKeyReleased(const sf::Event::KeyChanged& key)
{
	if (m_focusedWidget)
	{
		m_focusedWidget->onKeyReleased(key);
	}

#ifdef DEBUG
	if (key.code == DEBUG_INSIGHT_KEY) DEBUG_INSIGHT_KEY_PRESSED = false;
#endif
}


void Layout::onTextEntered(char32_t unichar)
{
	if (m_focusedWidget)
	{
		m_focusedWidget->onTextEntered(unichar);
	}
}


// API -----------------------------------------------------------------------

//----------------------------------------------------------------------------
bool Layout::focused() const //override
{
	return m_focusedWidget;
}

//----------------------------------------------------------------------------
bool Layout::focus(Widget* widget)
{
//cerr << "focus("<< widget <<") called" << (widget->isLayout() ? " (container!) ":"") <<endl;
	if (widget && widget->focusable() && widget->enabled())
	{
		// If another widget has already been focused, reset that first
		if (m_focusedWidget != widget)
			unfocus();

		m_focusedWidget = widget;
		widget->setActivationState(Focused);
		return true;
	}
	return false;
}

bool Layout::focusNext()
{
	// If the focused child is also a layout, then let it handle this locally first:
	if (m_focusedWidget && m_focusedWidget->isLayout())
	{
		if (m_focusedWidget->toLayout()->focusNext())
			return true;
	}

	// Still here? The child couldn't handle the focus change. Try further in this container then...

	Widget* start = m_focusedWidget ? next(m_focusedWidget) : begin();

	for (Widget* widget = start; widget != end(); widget = next(widget))
	{
		if (Layout* container = widget->toLayout(); container)
		{
			if (container->focusNext())
			{
				if (focus(container))
					return true;
				//!!??else
				//!!??	assert("shouldn't be here")? When exactly can this legally fail?
			}
		}
		else if (focus(widget))
		{
			return true;
		}
	}

	// If we end up here, no widget could be focused (despite possibly having set one as such -- clean it up then)!
	unfocus();
	return false;
}

bool Layout::focusPrevious()
{
	// If the focused child is also a layout, then let it handle this locally first:
	if (m_focusedWidget && m_focusedWidget->isLayout())
	{
		if (m_focusedWidget->toLayout()->focusPrevious())
			return true;
	}

	// Still here? The child couldn't handle the focus change. Try further in this container then...

	Widget* start = m_focusedWidget ? prev(m_focusedWidget) : rbegin();

	for (Widget* widget = start; widget != rend(); widget = prev(widget))
	{
		if (Layout* container = widget->toLayout(); container)
		{
			if (container->focusPrevious())
			{
				if (focus(container))
					return true;
				//!!??else
				//!!??	assert("shouldn't be here")? When exactly can this legally fail?
			}
		}
		else if (focus(widget))
		{
			return true;
		}
	}

	// If we end up here, no widget could be focused (despite possibly having set one as such -- clean it up then)!
	unfocus();
	return false;
}


//----------------------------------------------------------------------------
void Layout::unfocus()
{
	if (!m_focusedWidget) // No focused child
		return;

	// Note: do unfocus even if it got disabled in the meantime...

	auto widget = m_focusedWidget; // (just an alias for readability & consistency with other contexts)

	widget->setActivationState(Default);

	// Do it recursively for layouts! (!!??Should it be generalized to WidgetContainers??!!)
	if (widget->isLayout()) ((Layout*)widget)->unfocus();

	m_focusedWidget = nullptr;
}


//----------------------------------------------------------------------------
void Layout::hover(Widget* widget, float parent_x, float parent_y)
{
	if (m_hoveredWidget == widget) // Already hovered (or null)
		return;

	unhover(); // Cancel previous hover (even if this widget is disabled, of course!)

	if (!widget || !widget->enabled())
		return;

	m_hoveredWidget = widget;

	// Set it to "Hovered" only if not already focused:
	widget->setActivationState(widget == m_focusedWidget ? Focused : Hovered);

	widget->onMouseEnter(); //!!...(localMousePos)

	// Engage tooltip, if there's one
	//! No need to cancel the prev. active one, as hovering away from its owner should've already killed it.
	if (widget->m_tooltip)
		widget->m_tooltip->arm(parent_x, parent_y); // Parent of the tooltip owner!
}

//----------------------------------------------------------------------------
void Layout::unhover()
{
	if (!m_hoveredWidget) // No hovered child
		return;

	// Note: do unhover even if it got disabled in the meantime...

	auto widget = m_hoveredWidget; // (just an alias for readability & consistency with other contexts)

	// Do it recursively for layouts! (!!??Should it be generalized to WidgetContainers??!!)
	if (widget->isLayout()) ((Layout*)widget)->unhover();

//!!??	widget->onMouseMoved(x, y);

	// Set it back to "Focused" if it's been the focused child:
	widget->setActivationState(widget == m_focusedWidget ? Focused : Default);

	widget->onMouseLeave(); //!!...(localMousePos)

	// Cancel the tooltip, too, if active, unless multiple tooltips are enabled...
	if (!Theme::cfg.multiTooltips)
	{
		if (widget->m_tooltip)
			widget->m_tooltip->dismiss();
	}

	m_hoveredWidget = nullptr; //!!BEWARE: The trick in draw() for winning the tooltip Z-order assumes
	                           //!!m_hovered still set for the (last) tooltip, so this would kill the fadeout!... :-/
}

} // namespace
