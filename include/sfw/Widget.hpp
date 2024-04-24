#ifndef _SODRTYUN3045679MY450786NY3079Y78RYCHKLVJ_
#define _SODRTYUN3045679MY450786NY3079Y78RYCHKLVJ_

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
#include <string_view>
#include <concepts>
#include <functional>
#include <expected>

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
// Required for defining widget(...):
class Widget; template <std::derived_from<Widget> W = Widget> class WidgetPtr;

/*****************************************************************************

    Abstract (not in the formal C++ sense!) base class for widgets

    (See InputWidget as the base for interactive widgets.)

 *****************************************************************************/
//!!C++: No, can't just make this a template like this (for *optional* CRTP support):
//!!struct WidgetProxy{};
//!!template <class W = WidgetProxy>
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

	virtual bool focused() const; // Layouts will override it.

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

public: //!! WidgetRef needs these:
	Widget();
	virtual ~Widget();
protected:
	//!! These two horrible, depressing monstrosities only temporarily, until default-friendly
	//!! smart pointers are introduced (which will need valgrind etc. tests with raw pointers);
	//!! see #320!
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
	// Note: the only reason this is a member, not a free function, is
	// because it needs access to protected/private stuff (in both the
	// Widget and the GUI manager classes), and this way the (brittle)
	// friend declarations can be minimized.
	// Alas, it still needs to be public, so any free template functions
	// can also call it. (Well, at least it's hidden at the bottom of a
	// class; kinda private enough then, right?...)
	//


// ---- Proxied widget lookup/access helper API... ---------------------------
public:

	//!! Should be protected, but also used by WidgetPtr<W>, which can't be
	//!! declared friend in this non-template Widget class:
	[[nodiscard]] static Widget* find_proxied(std::string_view name, const Widget* w = nullptr);
	// If w == null, try the global default (singleton) GUI manager.
	// If w is set, use that to look up its actual manager object.

#ifdef _MSC_VER
	//!! Preparing (+ reminder) for Widget becoming a CRTP base...:
	template <std::derived_from<Widget> W = Widget, class Self>
	[[nodiscard]] auto find_widget(this Self&& self, std::string_view name) { return WidgetPtr<W>(name, (const W*) &self); }
#else //!! "Deducing this" of C++23 is not in my GCC (13.x) yet! :-/
	template <std::derived_from<Widget> W = Widget>
	[[nodiscard]] auto find_widget(std::string_view name) const { return WidgetPtr<W>(name, this); }
#endif

	// Like find_widget(...), but static, proxied via the default GUI mgr.
	// (and called `find` because Widget::find_widget would be lame...):
	template <std::derived_from<Widget> W = Widget>
	[[nodiscard]] static WidgetPtr<W> find(std::string_view name) { return WidgetPtr<W>( (W*)Widget::find_proxied(name) ); }

	// Null-safe (indirect) widget op. via named lookup
	template <std::derived_from<Widget> OtherWidget, typename F>
	auto call(const char* name, F&& f) const
		-> std::expected<typename std::invoke_result_t<std::decay_t<F>, OtherWidget*>,
		                 std::nullptr_t>
	{
		if (OtherWidget* w = this->template find_widget<OtherWidget>(name); w) {
		  if constexpr (std::is_void_v<std::invoke_result_t<std::decay_t<F>, OtherWidget*>>) {
			f(w);
			return {}; //! Not just `return;`: std::expected needs an init. list.
		  } else {
			return f(w);
		  }
		}

		return std::unexpected(nullptr);
	}

	/*!!C++: This won't help with auto-deducing the widget type of the callback's param.,
	//!! as lambdas (like [](SomeWidget*) { ... }) are never derived(!) from std::function<>... :-/
	template <std::derived_from<Widget> OtherWidget, typename F>
	auto call(const char* name, std::function<void(OtherWidget*)>&& f) const
		-> std::expected<typename std::invoke_result_t<std::decay_t<F>, OtherWidget*>,
		                 std::nullptr_t>
	{
		...
	}
	!!*/

	// Also a static version for proxying through the last default GUI manager:
	/*!!C++: Alas, overloading just by `static` is not possible. Must put it into sfw:: instead! :-/
	template <class W, typename F>
	static auto call(const char* name, F&& f) //!!?? Or const F&? Or even just F?...
		-> std::expected<typename std::invoke_result_t<std::decay_t<F>, W*>,
				std::nullptr_t>
	{
		if (auto w = (W*)Widget::find_proxied(name); w) {
			//!!C++: This sad conditional is to avoid a compiler error if f is void:
			if constexpr (std::is_void_v<std::invoke_result_t<std::decay_t<F>, W*>>) {
				f(w);
				return {}; //! Not just `return;`: std::expected needs an init. list.
			} else {
				return f(w);
			}
		}

		return std::unexpected(nullptr);
	}
	!!*/

	//!!--------------------------------------------------------------------------
	//!! These proxy getters/setters below would be more natural at InputWidget,
	//!! but this API should also be available via the main GUI manager, which
	//!! itself isn't an InputWidget, unfortunately. (And it can't just become
	//!! one either, as it's already a Layout (VBox).)
	//!!--------------------------------------------------------------------------

	//----------------------------------------------------------------------------
	// Returns std::expected, to be checked explicitly by the caller
	//
	//! NOTE: The arg. list is like this to ensure > 1 arguments, to avoid
	//!       colliding with the normal set(...), which only has 1.
	template <std::derived_from<Widget> OtherWidget, typename Arg>
	decltype(auto) set(const char* name, Arg&& arg)
	{
		return this->template call<OtherWidget>(name, [&](OtherWidget* w) {
			return w->set(std::forward<Arg>(arg));
		});
	}
	/*!! Example (just for the syntax) for a multi-arg case, nonentheless:
	template <std::derived_from<Widget> W, typename A, typename... Args>
	decltype(auto) set(const char* name, A&& arg1, Args&&... more_args)
	{
		return this->template call<W>(name, [&](auto* w) {
			return w->set(std::forward<A>(arg1), std::forward<Args>(more_args)...);
		});
	}
	!!*/

	template <std::derived_from<Widget> OtherWidget, typename Arg>
	decltype(auto) update(const char* name, Arg&& arg)
	{
		return this->template call<OtherWidget>(name, [&](OtherWidget* w) {
			return w->update(std::forward<Arg>(arg));
		});
	}

	//----------------------------------------------------------------------------
	// Returns std::expected, to be checked explicitly by the caller
	//
	//! NOTE: The normal get(...) has no args, so this should not collide with that.
	template <std::derived_from<Widget> OtherWidget>
	[[nodiscard]] auto get(const char* name)
	{
		return this->template call<OtherWidget>(name, [](OtherWidget* w) { return w->get(); });
	}

	//----------------------------------------------------------------------------
	// Returns the named widget's content if found, or a default (fallback) value
	//
	// Note: Should be deprecated if/when the C++23 monadic `.or_else(...)` form
	// becomes the dominant style!
	template <std::derived_from<Widget> OtherWidget, typename TRet>
	[[nodiscard]] auto get_or(const char* name, TRet defval) //!! std::expected's or_else is still too new (e.g. it's not there in my MSVC either)
	{
		auto result = this->template call<OtherWidget>(name, [](OtherWidget* w) { return w->get(); });
		return result ? result.value() : defval;
	}
	//----------------------------------------------------------------------------
	// Plus a convenience synonym of get_or (as yet another `get` overload)
	//
	//!!C++: Sometimes failed to compile inside widget calbbacks, as w->get<Other>("name", ...)
	//!! -- even though w->set<Other>("name", ...) still worked, and so did gui.get<Other>("name", ...)! :-o
	template <std::derived_from<Widget> OtherWidget, typename TRet>
	[[nodiscard]] auto get(const char* name, TRet defval)
	{ return this->template get_or<OtherWidget, TRet>(name, std::forward<decltype(defval)>(defval)); }

	//----------------------------------------------------------------------------
	//! Only use this flavor if you're ABSOLUTELY sure that `name` does exist,
	//! otherwise that .value() would throw bad_expected_access! (Well you may just
	//! catch that one, too -- but that kinda defeats the purpose of using this in
	//! the first place.)
	//!
	//!!??C++:
	//!! std::expected can't handle references returned by get(), so no point in using
	//!! `decltype(auto)` here, right? Also, call() itself already decays it anyway...
	//!! Also:
	//!! Since .value() returns a reference, MSVC warns about "returning address of
	//!! local variable or temporary"... Is this a matter of copy elision? It's way
	//!! too shaky, nonetheless, so better not trying to be reference-friendly here!
	//!! No decltype(auto), just auto then...
	//!!
	template <std::derived_from<Widget> OtherWidget>
	[[nodiscard]] auto get_v(const char* name) // Called ..._v to rhyme with the (cringy) std value-returning templates.
	{
		return this->template get<OtherWidget>(name).value();
	}

}; // class Widget

} // namespace sfw

#endif // _SODRTYUN3045679MY450786NY3079Y78RYCHKLVJ_
