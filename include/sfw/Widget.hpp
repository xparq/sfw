#ifndef _SFW_WIDGET_HPP_
#define _SFW_WIDGET_HPP_

#include "sfw/Event.hpp"
#include "sfw/ActivationState.hpp"

//!!#include "sfw/Widgets/Tooltip.hpp" // See forw. decl. below instead...
//!!#include "sfw/WidgetContainer.hpp" // See forw. decl. below instead...
//!!#include "sfw/Layout.hpp"          // See forw. decl. below instead...
//!!#include "sfw/GUI-main.hpp"        // See forw. decl. below instead...

#include "sfw/Gfx/Render.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Window/Event.hpp>

#include <string>

#ifdef DEBUG
#
#  include <SFML/Graphics/Color.hpp> // for draw_outline()
#
#endif


namespace sfw
{
class WidgetContainer;
class Layout;
class Tooltip;
class GUI;

/*****************************************************************************

    Abstract base for widgets

    See InputWidget as the base for "actually useful" widgets, though. ;)

 *****************************************************************************/
class Widget : public gfx::Drawable, public Event::Handler
{
public:
	// Widget geometry
	Widget* setPosition(const sf::Vector2f& pos);
	Widget* setPosition(float x, float y);
	const sf::Vector2f& getPosition() const; //!!?? Should just return Vector2f by val.?

	// Don't use these, unless absolutely necessary & signed the waiver!...
	// (They trigger reflow, so might cause infinite loops if called nonchalantly.)
	// Widget sizing is fully automatic anyway.
	Widget* setSize(const sf::Vector2f& size);
	Widget* setSize(float width, float height);
	const sf::Vector2f& getSize() const; //!!?? Should just return Vector2f by val.?

	// Queries:

	// Check if a point belongs to the widget
	bool contains(const sf::Vector2f& point) const;

	// Enable/disable processing (user) events
	// (Not just inputs, but also outputs like triggering user callbacks.)
	Widget* enable(bool state = true);
	bool    enabled() const;
	Widget* disable()        { return enable(false); }
	bool    disabled() const { return !enabled(); }

	virtual bool focused() const; // Layouts may need to override it.

	// Set/Reset widget name
	//
	// Notes:
	// - If a widget is not assigned a name explicitly, it will have a unique
	//   default ID (e.g. the hex. representation of its (original) mem. address).
	//   (Note: while widgets are not copied/moved after being added to the GUI,
	//   they can be during setup.)
	//
	// - If the name has already been assigned to another widget, the newly named
	//   widget will win, stealing the name from the other widget, which will then
	//   revert to having a default ID instead.
	//   (Perhaps unsurprisingly, this behavior might change in the future. ;) )
	void setName(const std::string&);

	// Get the optional name of a widget (or its internal ID, if unnamed)
	// If a widget is specified then that one will be looked up, not the current one.
	// Note: This may be a slow operation (intended mainly for diagnostics).
	std::string getName(Widget* widget = nullptr) const;

	Widget* setTooltip(const std::string& text);

protected:
//----------------------
friend class WidgetContainer;
friend class Layout;
friend class GUI;
//!!Temporarily, until Widget becomes WidgetContainer:
friend class Tooltip; // just to access getMain() via Tooltip::m_owner->!
//----------------------

	Widget();
	virtual ~Widget();
	//!! These two horrible, depressing monstrosities only temporarily, until default-friendly
	//!! smart pointers are introduced (which will need to follow thorough valgrind etc. tests
	//!! with raw pointers); see #320!
		Widget(Widget&&);
		Widget(const Widget&);

	sf::Vector2f getAbsolutePosition() const;
		// Window coordinates, not relative to the GUI Main rect.!
		// (So, even when the GUI itself is offset, it won't affect this.)

	void setActivationState(ActivationState state);
	ActivationState getActivationState() const;

	// Set/check if the widget can be focused (i.e. process & trigger user events)
	void setFocusable(bool focusable); //!!setInteractive, as it's all about taking user inputs!
	bool focusable() const;

	// Get the widget typed as a Layout, if applicable
	virtual Layout* toLayout() { return nullptr; }
	bool isLayout() { return toLayout() != nullptr; }

	// Set/get the parent (container) of the widget
	//!!But Widget itself should probably be a WidgetContainer, to support composition (sub-widgets)!
	void setParent(WidgetContainer* parent);
	WidgetContainer* getParent() const { return m_parent; }

	// Is this a root widget?
	// Root widgets are those that have no parent (e.g. newly created ones
	// that have not yet been added to containers, or widgets not yet added
	// to the GUI, or the main GUI object itself).
	bool isRoot() const;

	// Note: free-standing (dangling) widgets (those that have been created
	// but not attached to other widgets, or the GUI, are their own root.
	Widget* getRoot() const;

	// Is this the GUI's "main widget"?
	bool isMain() const;

	// Get the GUI object (the "main widget")
	// Free-standing widgets (those that have not yet been attached to the
	// the GUI can't access the Main object, and will return null.
	GUI* getMain() const;

	// Recursive traversal of all the widget's descendants (if any)
	// (Does not include the target widget itself.)
	// Note: this operation is not strictly related to widget *containers*,
	// despite only WidgetContainer implementing it currently.
	// This uniform interface is provided here on the Widget level a) for
	// the convenience of uniform access, and b) to support compound widget
	// types that don't want to become containers per se, but just have an
	// easily manageable internal structure of sub-widgets.
	//!! (Not sure this latter makes sense, though: maybe that's OK to just be a container then!
	//!! Also: complex assemblies may have multiple widget trees, too, not just this -- also
	//!! implicit... -- one.)
	virtual void traverse(const std::function<void(Widget*)>&) {}
	virtual void ctraverse(const std::function<void(const Widget*)>&) const {}

private:
	virtual void recomputeGeometry() {} // Also called by some of the friend classes

	// -------- Callbacks... (See event.hpp for the generic ones!)
	virtual void onActivationChanged(ActivationState) {}
	virtual void onResized() {}
	virtual void onThemeChanged() {}

private:
	WidgetContainer* m_parent = nullptr;
	Widget* m_previous = nullptr;
	Widget* m_next = nullptr;

	bool m_focusable;
	ActivationState m_activationState;

	sf::Vector2f m_position;
	sf::Vector2f m_size;
	sf::Transform m_transform;

	Tooltip* m_tooltip = nullptr;

#ifdef DEBUG
public:
	void draw_outline(const gfx::RenderContext& ctx, sf::Color outlinecolor = sf::Color::Red,
		sf::Color fillcolor = sf::Color::Transparent) const; // Well, with fillColor, now it can also do interior, not just "outline"
#endif

// ---- Misc. hackery... -----------------------------------------------------

	// Rotation, scale and translation adjustments for the widget-local
	// gfx. context, relative to the parent's
	//!!Shouldn't really be public, or even exist, if we had a proper render-context API already...
	public:
	const sf::Transform& getTransform() const;

	// Internal helper to shield this header from some extra dependencies
	// See the freestanding functions that need this, below the class!
	//
	// Note: the only reason this is a (public static) member, not a free
	// fn. itself is because it needs access to protected/private stuff
	// in both the Widget and the GUI classes, and this way the (brittle)
	// friend declarations can be minimized.
	// Alas, it needs to be public in exchange, so those free template functions
	// can use it... (Well, at least it's hidden at the bottom of a class;
	// kinda considered private enough then, right?...)
	//
	public:
	static Widget* getWidget_proxy(const std::string& name, const Widget* w = nullptr);
	// If w == null, try the global default (singleton) GUI manager.
	// If w is set, use that to look up its actual manager object.

}; // class Widget


//----------------------------------------------------------------------------
// Misc...
//----------------------------------------------------------------------------
// Find widget by name
// Returns nullptr if the name was not found.
template <class W = Widget> W* getWidget(const std::string& name, const Widget* w) { return (W*)Widget::getWidget_proxy(name, w); }
template <class W = Widget> W* getWidget(const std::string& name, const Widget& w) { return (W*)Widget::getWidget_proxy(name, &w); }
// Plus this one for convenience: the last created GUI instance will register itself as the default
// (singleton) widget manager, so we can talk to it, provided we know for sure there can't be
// another one in our app.
template <class W = Widget> W* getWidget(const std::string& name) { return (W*)Widget::getWidget_proxy(name); }
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !!
  !! Why aren't these above just Widget:: (or GUI::) members?
  !! Well, this remnant, removed from the class (as (part) of #322) may help
  !! illuminate it:
  !!
	Widget* getWidget(const std::string& name) const;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!
	//!! JESUS CHRIST, C++, WTF?! :-ooo Get your shit together! ;) See #322...
	//!! Not even this sad workaround can work (except in MSVC, amen to that...)!
	//!! See the freestanding getWidget() functions after this class for the current
	//!! alternative solution.
	//!!
	//!!	template <class W = Widget>
	//!!	W* getWidget(const std::string& name) const { return (W*)getWidget_internal(name); }
	//!!		//! Would be nice to have it templated directly, but that would introduce
	//!!		//! a nasty cross-dependency on the top-level GUI stuff.
	//!!		Widget* getWidget_internal(const std::string& name) const;
	//!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

} // namespace

#endif // _SFW_WIDGET_HPP_
