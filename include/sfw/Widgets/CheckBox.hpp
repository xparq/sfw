#ifndef GUI_CHECKBOX_HPP
#define GUI_CHECKBOX_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Shapes/Box.hpp"
#include "sfw/Gfx/Shapes/CheckMark.hpp"

namespace sfw
{

/**
 * The CheckBox is a widget for enabling/disabling an option.
 * The callback is triggered when the checkbox is checked or unchecked.
 */
class CheckBox: public Widget
{
public:
    CheckBox(bool checked = false);
    CheckBox(std::function<void(CheckBox*)> callback, bool checked = false);

    bool isChecked() const;

    void check(bool checked);

    CheckBox* setCallback(std::function<void(CheckBox*)> callback);
    CheckBox* setCallback(std::function<void()> callback)         { return (CheckBox*) Widget::setCallback(callback); }

private:
    void draw(const gfx::RenderContext& ctx) const override;

    // Callbacks
    void onStateChanged(WidgetState state) override;
    void onMouseReleased(float x, float y) override;
    void onKeyPressed(const sf::Event::KeyEvent& key) override;

    Box m_box;
    CheckMark m_checkmark;
    bool m_checked;
};

} // namespace

#endif // GUI_CHECKBOX_HPP
