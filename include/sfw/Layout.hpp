#ifndef _DOIFUU98DTHUMU985M85Y987NW4579631BRT65Z1286GYMC438Y_
#define _DOIFUU98DTHUMU985M85Y987NW4579631BRT65Z1286GYMC438Y_


#include "sfw/WidgetContainer.hpp"

#include <functional>


namespace sfw
{

/*****************************************************************************
   Generic widget manager to control the positioning, plus the event dispatching
   and various state updates (hovered, focused etc.) of its children

   It's used to implement all the actual layout controllers like HBox, VBox,
   Form etc. (but it's not an abstract base in the C++ sense)

   A Layout is also a Widget.
 *****************************************************************************/
class Layout: public WidgetContainer
{
public:
	bool focused() const override;
	void unfocus(); // Remove focus from the focused child (recursively)
	                // Made public to support apps with multiple GUI panels/windows (#368)
protected:
	Layout();

	// Override the generic Widget API (which just returns null):
	Layout* toLayout() override { return this; }

	// Set the focus on a child widget, if applicable
	// Returns true if the widget took the focus, otherwise false.
	bool focus(Widget* widget);
	bool focusNext();
	bool focusPrevious();

	void hover(Widget* widget, float parent_x, float parent_y); //!! The coords. are a kludge for tooltip support...
	void unhover(); // Unhover last hovered child

// ---- Callbacks ------------------------------------------------------------
	void draw(const gfx::RenderContext& ctx) const override;

	void onActivationChanged(ActivationState state) override;
	void onMouseMoved(float x, float y) override;
	void onMousePressed(float x, float y) override;
	void onMouseReleased(float x, float y) override;
	void onMouseWheelMoved(int delta) override;
	void onKeyPressed(const event::KeyCombination& key) override;
	void onKeyReleased(const event::KeyCombination& key) override;
	void onTextEntered(char32_t codepoint) override;

private:
	Widget* m_hoveredWidget;
	Widget* m_focusedWidget;

}; // class Layout

} // namespace sfw


#endif // _DOIFUU98DTHUMU985M85Y987NW4579631BRT65Z1286GYMC438Y_
