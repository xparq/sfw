#ifndef GUI_WIDGET_HPP
#define GUI_WIDGET_HPP

#include "sfw/WidgetState.hpp"
#include "sfw/Gfx/Render.hpp"
//!!#include "sfw/Layout.hpp"   // See forw. decl. below instead...
//!!#include "sfw/GUI-main.hpp" // See forw. decl. below instead...

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Cursor.hpp>

#include <functional>
#include <type_traits>

namespace sfw
{
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
     * Widget's position
     */
    void setPosition(const sf::Vector2f& pos);
    void setPosition(float x, float y);
    const sf::Vector2f& getPosition() const;

    sf::Vector2f getAbsolutePosition() const;

    /**
     * Get widget's dimensions
     */
    const sf::Vector2f& getSize() const;

    /**
     * Check if a point is inside the widget
     */
    bool containsPoint(const sf::Vector2f& point) const;

    /**
     * Check if the widget can be selected and trigger events
     */
    bool isSelectable() const;

    /**
     * Check if the widget is currently focused
     */
    bool isFocused() const;

    /**
     * Set a function to be called when this widget is triggered
     */
    template <class W> W* setCallback(std::function<void(W*)> callback)
        requires (std::is_base_of_v<Widget, W>)
    {
        m_callback = [this, callback]{ return callback(this); };
        return (W*)this;
    }

    template <class W> W* setCallback(std::function<void()> callback)
        requires (std::is_base_of_v<Widget, W>)
    {
        m_callback = callback;
        return (W*)this;
    }
    // The actual (derived) widgets should then provide specialized variants as:
    // InterestingWidget* setCallback(std::function<void()> callback) { return Widget::setCallback<InterestingWidget>(callback); }
    // InterestingWidget* setCallback(std::function<void(InterestingWidget*)> callback)
    // {
    //     return (InterestingWidget*) Widget::setCallback( [callback] (Widget* w) { callback( (InterestingWidget*)w ); });
    // }
    //
    // These are also added here for completeness, and to help client code
    // that uses legacy widgets (via abundant casting...) that still don't have
    // their own specialized API variants (of the templates abobe):
    Widget* setCallback(std::function<void(Widget*)> callback);
    Widget* setCallback(std::function<void()> callback);

protected:
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

    void setSize(const sf::Vector2f& size);
    void setSize(float width, float height);

//----------------------
friend class GUI;
friend class Layout;
friend class FormLayout;
friend class HBoxLayout;
friend class VBoxLayout;
//----------------------

    void setSelectable(bool selectable);

    void triggerCallback();

    void setState(WidgetState state);
    WidgetState getState() const;

    /**
     * Set the widget's container (parent)
     */
    void setParent(Layout* parent);
    Layout* getParent() { return m_parent; }

    bool isRoot(); // Root widgets have no parent (they're containers themselves)
    bool isMain(); // Root widget that's also the "Main widget"

    /**
     * Get the widget typed as a Layout if applicable
     * Used to check if the widget is a container (Layout and its subclasses)
     */
    virtual Layout* toLayout() { return nullptr; }

    void centerText(sf::Text& text);

    const sf::Transform& getTransform() const;

    /**
     * The "root" of a widget is a parent that has no parent.
     * It can also be the Main GUI object, if its parent is set to itself.
     * Note: free-standing ("dangling") widgets (those that have been created
     * but not yet attached to the GUI, have a root, but not a Main object.
     */
    Widget* rootWidget();

    GUI* GUIMain(); // nullptr for free-standing widgets

private:
    /**
     * Widgets that are also Layouts would iterate their children via this
     * interface. The reason this is defined here in Widget, not there, is:
     *   a) parents of Widgets & Layouts shouldn't care about the difference,
     *   b) Widget types other than Layout may have children in the future.
     * The traversal is recursive (not just a single-level iteration).
     */
    virtual void traverseChildren(std::function<void(Widget*)> f) { f(this); }

    virtual void recomputeGeometry() {}

    Layout* m_parent;
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
