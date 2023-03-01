#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP

#include "sfw/Widget.hpp"
#include "sfw/Shapes/ItemBox.hpp"

namespace gui
{

/**
 * The Button widget is a simple press button.
 * The callback is triggered when the button is clicked/activated.
 */
class Button: public Widget
{
public:
    Button(const sf::String& string);
    Button(const sf::String& string, std::function<void()> callback);
    Button(const sf::String& string, std::function<void(Button*)> callback);

    /**
     * Set the displayed button label
     */
    void setString(const sf::String& string);

    const sf::String& getString() const;

    // See Widget.hpp for the templates of these:
    Button* setCallback(std::function<void()> callback)         { return Widget::setCallback<Button>(callback); }
    Button* setCallback(std::function<void(Button*)> callback);

protected:
    // Callbacks
    void onStateChanged(WidgetState state) override;
    void onMouseMoved(float x, float y) override;
    void onMousePressed(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onKeyReleased(const sf::Event::KeyEvent& key) override;

private:
    void draw(const gfx::RenderContext& ctx) const override;

    ItemBox<sf::Text> m_box;
};

}

#endif // GUI_BUTTON_HPP
