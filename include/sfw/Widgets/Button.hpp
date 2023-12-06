#ifndef _SFW_BUTTON_HPP_
#define _SFW_BUTTON_HPP_

#include "sfw/InputWidget.hpp"
#include "sfw/Gfx/Elements/Text.hpp"
#include "sfw/Gfx/Elements/ItemBox.hpp"
#include "sfw/Gfx/Color.hpp"

#include <string>

namespace sfw
{

class Button: public InputWidget<Button>
/*===========================================================================
  Simple generic push-button

  The update callback is triggered when the mouse button is released (after
  having clicked the widget first), or when pressing Space/Enter (while
  being focused).
 ===========================================================================*/
{
public:
	explicit Button(const std::string& text);
	Button(const std::string& text, std::function<void()> callback);
	Button(const std::string& text, std::function<void(Button*)> callback);

	Button* setText(const std::string& text);
	std::string getText() const;

	Button* setColor(sf::Color); // Overall tint, except the label
	Button* setTextColor(sf::Color);

	Button* click(); // "Macro" for automation

private:
	void draw(const gfx::RenderContext& ctx) const override;

	void recomputeGeometry() override;

	// Callbacks
	void onActivationChanged(ActivationState state) override;
	void onMouseMoved(float x, float y) override;
	void onMousePressed(float x, float y) override;
	void onMouseReleased(float x, float y) override;
	void onKeyPressed(const sf::Event::KeyEvent& key) override;
	void onKeyReleased(const sf::Event::KeyEvent& key) override;
	void onThemeChanged() override;

	ItemBox<Text> m_box;
};

} // namespace

#endif // _SFW_BUTTON_HPP_
