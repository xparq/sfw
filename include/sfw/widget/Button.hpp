#ifndef _MDEIURFYHHTNUIWHWIDMUTFHYIUEG9849836798435GY7G_
#define _MDEIURFYHHTNUIWHWIDMUTFHYIUEG9849836798435GY7G_


#include "sfw/InputWidget.hpp"
#include "sfw/gfx/element/Text.hpp"
#include "sfw/gfx/element/ItemBox.hpp"
#include "sfw/gfx/Color.hpp"

#include <string_view>


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
	explicit Button(std::string_view text);
	Button(std::string_view text, std::function<void()> callback);
	Button(std::string_view text, std::function<void(Button*)> callback);

	Button* setText(std::string_view text);
	std::string getText() const;

	Button* setColor(Color); // Overall tint, except the label
	Button* setTextColor(Color);

	Button* click(); // "Macro" for automation

private:
	void draw(const gfx::RenderContext& ctx) const override;

	void recomputeGeometry() override;

	// Callbacks
	void onActivationChanged(ActivationState state) override;
	void onMouseMoved(float x, float y) override;
	void onMousePressed(float x, float y) override;
	void onMouseReleased(float x, float y) override;
	void onKeyPressed(const sf::Event::KeyChanged& key) override;
	void onKeyReleased(const sf::Event::KeyChanged& key) override;
	void onThemeChanged() override;

	ItemBox<gfx::Text> m_box;

}; // class Button

} // namespace sfw


#endif // _MDEIURFYHHTNUIWHWIDMUTFHYIUEG9849836798435GY7G_
