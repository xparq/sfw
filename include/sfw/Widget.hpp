#ifndef GUI_WIDGET_HPP
#define GUI_WIDGET_HPP

#include "sfw/WidgetState.hpp"
#include "sfw/Gfx/Render.hpp"
//!!#include "sfw/WidgetContainer.hpp" // See forw. decl. below instead...
//!!#include "sfw/Layout.hpp" // See forw. decl. below instead...
//!!#include "sfw/GUI-main.hpp" // See forw. decl. below instead...

#include <functional>
#include <optional>
#include <variant>
#include <string>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Window/Event.hpp>

#ifdef DEBUG
#
#  include <SFML/Graphics/Color.hpp>
#
#endif


namespace sfw
{
class WidgetContainer;
class Layout;
class GUI;

/**
 * Abstract base class for gui widgets
 */
class Widget: public gfx::Drawable
{
public:
    /**
     * Widget position
     */
    Widget* setPosition(const sf::Vector2f& pos);
    Widget* setPosition(float x, float y);

    const sf::Vector2f& getPosition() const;
    const sf::Vector2f& getSize() const;
    const sf::Transform& getTransform() const;

    /**
     * Check if a point is inside the widget
     */
    bool containsPoint(const sf::Vector2f& point) const;

    bool focused() const;

    // Enable/disable processing user events (not just inputs, but also outputs
    // like triggering user callbacks)
    Widget* enable(bool state = true);
    bool    enabled() const;
    Widget* disable() { return enable(false); }
    bool    disabled() const { return !enabled(); }

    /**
     * Set a function to be called when the "value" of the widget is changed
     */
    private:
        using Callback_void = std::optional<std::function<void()>>;
        using Callback_w    = std::optional<std::function<void(Widget*)>>;
public:
    using Callback  = std::variant<Callback_w, Callback_void>;
    Widget* setCallback(Callback callback);

    // The actual (derived) widgets should then provide specific overloads, as:
    // SomeWidget* setCallback(std::function<void(SomeWidget*)> callback)
    // {
    //     return (SomeWidget*) Widget::setCallback([callback] (Widget* w) { callback( (SomeWidget*)w ); });
    // }
    // SomeWidget* setCallback(std::function<void()> callback)
    // {
    //      return (SomeWidget*) Widget::setCallback(callback);
    // }


    /*************************************************************************
      Set/Reset the internal name of the widget

      Notes:
      - If a widget is not assigned a name explicitly, it will have a unique
        default ID (e.g. the hex. representation of its address).

      - If the name has already been assigned to another widget, it will lose
        its explicit name (reverting to the default), and the new widget will
        take over, having that name thereafter.
     */
    void setName(const std::string& name);

    /**
      Get the internal name of a widget (or its default ID if hasn't been named)

      If no widget (address) is specified, it means this widget.

      Note:
      - This may be a slow operation, intended mainly for diagnostic use!
     */
    std::string getName(Widget* widget = nullptr) const;

    /**
      Find a registered widget by name
      Returns nullptr if the name was not found.
     */
    Widget* getWidget(const std::string& name) const;

    /**
      Method-chaining helper for setting up nested widget structures
      (Note: getParent is not public, plus this may get arguments in the future.)
    */
    WidgetContainer* parent(/*!!enum FailMode mode = or_burn*/) { return getParent(); }

protected:
//----------------------
friend class WidgetContainer;
friend class Layout;
friend class GUI;
//----------------------

    Widget();
    virtual ~Widget() = default;

    void setSize(const sf::Vector2f& size);
    void setSize(float width, float height);

    sf::Vector2f getAbsolutePosition() const;

    void setState(WidgetState state);
    WidgetState getState() const;

    /**
     * Set/check if the widget can be focused (i.e. process & trigger user events)
     */
    void setFocusable(bool focusable); //!!setInteractive, as it's all about taking user inputs!
    bool focusable() const;

    void triggerCallback();

    /**
     * Get the widget typed as a Layout, if applicable
     */
    virtual Layout* toLayout() { return nullptr; }

    /**
     * Set/get the parent (container) of the widget
     */
    void setParent(WidgetContainer* parent);
    WidgetContainer* getParent() const { return m_parent; }

    /**
     * Is this a root widget?
     * Root widgets are those that have no parent (e.g. newly created containers
     * that have not yet been attached to the GUI).
     */
    bool isRoot() const;

    /**
     * The "root" of a widget is a parent that has no parent.
     * It can also be the Main GUI object, if its parent is set to itself.
     * Note: free-standing ("dangling") widgets (those that have been created
     * but not yet attached to the GUI, have a root, but not a Main object.
     */
    Widget* getRoot() const;

    /**
     * Is this the main GUI widget?
     */
    bool isMain() const;

    /**
     * Get the main GUI widget
     * (or null for free-standing widgets that have not yet been attached
     * to the GUI; but avoid that, as it will be considered invalid later!)
     */
    GUI* getMain() const;

    // Uniform node-level interface for recursive traversal of the widget tree
    virtual void traverseChildren(const std::function<void(Widget*)>&) {}


private:
    virtual void recomputeGeometry() {} // Can be requested by friend widgets, too

    // Callbacks
    virtual void onStateChanged(WidgetState state);
    virtual void onMouseEnter();
    virtual void onMouseLeave();
    virtual void onMouseMoved(float x, float y);
    virtual void onMousePressed(float x, float y);
    virtual void onMouseReleased(float x, float y);
    virtual void onMouseWheelMoved(int delta);
    virtual void onKeyPressed(const sf::Event::KeyEvent& key);
    virtual void onKeyReleased(const sf::Event::KeyEvent& key);
    virtual void onTextEntered(char32_t unichar);
    virtual void onThemeChanged();
    virtual void onResized();
    virtual void onTick();

    WidgetContainer* m_parent;
    Widget* m_previous;
    Widget* m_next;

    WidgetState m_state;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    bool m_focusable;
    Callback m_callback;
    sf::Transform m_transform;

#ifdef DEBUG
public:
	void draw_outline(const gfx::RenderContext& ctx, sf::Color outlinecolor = sf::Color::Red, sf::Color fillcolor = sf::Color::Transparent) const;
#endif
};


} // namespace

#endif // GUI_WIDGET_HPP
