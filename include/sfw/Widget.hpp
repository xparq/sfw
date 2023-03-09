#ifndef GUI_WIDGET_HPP
#define GUI_WIDGET_HPP

#include "sfw/WidgetState.hpp"
#include "sfw/Gfx/Render.hpp"
//!!#include "sfw/WidgetContainer.hpp" // See forw. decl. below instead...
//!!#include "sfw/Layout.hpp" // See forw. decl. below instead...
//!!#include "sfw/GUI-main.hpp" // See forw. decl. below instead...

#include <functional>
#include <string>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Window/Event.hpp>

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
    Widget();
    virtual ~Widget() {}

    /**
     * Widget position
     */
    void setPosition(const sf::Vector2f& pos);
    void setPosition(float x, float y);
    const sf::Vector2f& getPosition() const;

    const sf::Vector2f& getSize() const;

    /**
     * Check if a point is inside the widget
     */
    bool containsPoint(const sf::Vector2f& point) const;

    /**
     * Check if the widget can be selected and trigger events
     */
    bool isSelectable() const;

    bool isFocused() const;

    /**
     * Set a function to be called when this widget is triggered
     */
    Widget* setCallback(std::function<void(Widget*)> callback);
    Widget* setCallback(std::function<void()> callback);

    // The actual (derived) widgets should then provide specific overloads, as:
    // InterestingWidget* setCallback(std::function<void(InterestingWidget*)> callback)
    // {
    //     return (InterestingWidget*) Widget::setCallback([callback] (Widget* w) { callback( (InterestingWidget*)w ); });
    // }
    // InterestingWidget* setCallback(std::function<void()> callback)
    // {
    //      return (InterestingWidget*) Widget::setCallback(callback);
    // }

    /**
      * The internal name of the widget (or its hex address if it wasn't named)
      * (This may be a slow operation, intended mainly for diagnostic use!
      * Basically that's why it's not just called `name()`. ;) )
      */
    std::string getName() const;

    /**
      * Find (some other) widget by name, if previously registered (or null if not)
      */
    Widget* get(const std::string& name) const { return find(name); }


protected:
//----------------------
friend class GUI;
friend class WidgetContainer;
friend class Layout;
friend class VBox;
friend class HBox;
friend class Form;
//----------------------

    void setSize(const sf::Vector2f& size);
    void setSize(float width, float height);

    sf::Vector2f getAbsolutePosition() const;

    const sf::Transform& getTransform() const;

    void setState(WidgetState state);
    WidgetState getState() const;

    void setSelectable(bool selectable);

    void triggerCallback();

    /**
     * Get the widget typed as a Layout, if applicable
     */
    virtual Layout* toLayout() { return nullptr; }

    /**
     * Set the parent (container) of the widget
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

    /**
     * name->widget lookup (implemented in GUI::Main)
     */
    Widget* find(const std::string& name) const;

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
    virtual void onTextEntered(uint32_t unicode);
    virtual void onThemeChanged();
    virtual void onResized();


    WidgetContainer* m_parent;
    Widget* m_previous;
    Widget* m_next;

    WidgetState m_state;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    bool m_selectable;
    std::function<void()> m_callback;
    sf::Transform m_transform;

#ifdef DEBUG
public:
    void draw_outline(const gfx::RenderContext& ctx) const;
#endif
};


} // namespace

#endif // GUI_WIDGET_HPP
