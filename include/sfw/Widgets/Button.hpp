#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Elements/Text.hpp"
#include "sfw/Gfx/Elements/ItemBox.hpp"

#include <string>

namespace sfw
{

/**
 * The Button widget is a simple press button.
 * The callback is triggered when the button is activated (e.g. clicked).
 */
class Button: public Widget
{
public:
    explicit Button(const std::string& text);
    Button(const std::string& text, std::function<void()> callback);
    Button(const std::string& text, std::function<void(Button*)> callback);

    void setText(const std::string& text);
    std::string getText() const;

    Button* setCallback(std::function<void(Button*)> callback);
    Button* setCallback(std::function<void()> callback)         { return (Button*) Widget::setCallback(callback); }

private:
    void draw(const gfx::RenderContext& ctx) const override;

    void recomputeGeometry() override;

    // Callbacks
    void onStateChanged(WidgetState state) override;
    void onMouseMoved(float x, float y) override;
    void onMousePressed(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onKeyReleased(const sf::Event::KeyEvent& key) override;
    void onThemeChanged() override;

    ItemBox<Text> m_box;
};

} // namespace

#endif // GUI_BUTTON_HPP
