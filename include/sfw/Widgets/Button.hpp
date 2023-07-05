#ifndef SFW_BUTTON_HPP
#define SFW_BUTTON_HPP

#include "sfw/InputWidget.hpp"
#include "sfw/Gfx/Elements/Text.hpp"
#include "sfw/Gfx/Elements/ItemBox.hpp"

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

	Button* click(); // "Macro" for automation

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

#endif // SFW_BUTTON_HPP
