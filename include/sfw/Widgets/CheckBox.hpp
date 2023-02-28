#ifndef GUI_CHECKBOX_HPP
#define GUI_CHECKBOX_HPP

#include "sfw/Widget.hpp"
#include "sfw/Shapes/Box.hpp"
#include "sfw/Shapes/Cross.hpp"

namespace gui
{

/**
 * The CheckBox is a widget for enabling/disabling an option.
 * The callback is triggered when the checkbox is checked or unchecked.
 */
class CheckBox: public Widget
{
public:
    CheckBox(bool checked = false);

    bool isChecked() const;

    void check(bool checked);

    // See Widget.hpp for the templates of these:
    CheckBox* setCallback(std::function<void()> callback)         { return Widget::setCallback<CheckBox>(callback); }
    CheckBox* setCallback(std::function<void(CheckBox*)> callback);

protected:
    // Callbacks
    void onStateChanged(WidgetState state) override;
    void onMouseReleased(float x, float y) override;
    void onKeyPressed(const sf::Event::KeyEvent& key) override;

private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    Box m_box;
    Cross m_cross;
    bool m_checked;
};

}

#endif // GUI_CHECKBOX_HPP
