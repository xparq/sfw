#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Shapes/ItemBox.hpp"

namespace sfw
{

/**
 * The Button widget is a simple press button.
 * The callback is triggered when the button is clicked/activated.
 */
class Button: public Widget
{
public:
    explicit Button(const sf::String& string);
    Button(const sf::String& string, std::function<void()> callback);
    Button(const sf::String& string, std::function<void(Button*)> callback);

    /**
     * Set the displayed button label
     */
    void setString(const sf::String& string);

    const sf::String& getString() const;

    Button* setCallback(std::function<void(Button*)> callback);
    Button* setCallback(std::function<void()> callback)         { return (Button*) Widget::setCallback(callback); }

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

} // namespace

#endif // GUI_BUTTON_HPP
