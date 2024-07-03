#ifndef _IODUIWEHDGJHDJKFHGNDJKHGNIUWHER8UIGH45_
#define _IODUIWEHDGJHDJKFHGNDJKHGNIUWHER8UIGH45_


#include "sfw/InputWidget.hpp"
#include "sfw/gfx/element/Text.hpp"
#include "sfw/gfx/element/Texture.hpp"
#include "sfw/gfx/element/Font.hpp"

#include <string_view>

#include <SFML/Graphics/Sprite.hpp>


namespace sfw
{

/*===========================================================================
  Push-button with a texture and a text label
  (See Button!)
 ===========================================================================*/
class ImageButton: public InputWidget<ImageButton>
{
public:
	ImageButton(const /*!!gfx::!!*/Texture& texture, std::string_view label = "");

	ImageButton* setText(std::string_view label);
	std::string  getText() const;

	ImageButton* setSize(fVec2 size);
		// The default button size is the same as that of the image, but
		// an explicit setSize call will rescale the image to the new size.
	//!!?? Shouldn't this be virtual then?! Not even the base Widget setSize is!

	ImageButton* setFont(const /*!!gfx::!!*/Font& font);
	const /*!!gfx::!!*/Font& getFont() const;

	ImageButton* setTextSize(size_t size);
	ImageButton* setTextStyle(sf::Text::Style style);
	ImageButton* setTextColor(sf::Color color);

	ImageButton* setTexture(const /*!!gfx::!!*/Texture& texture);
		// Also resets the scaling; see setSize!

private:
	void draw(const gfx::RenderContext& ctx) const override;

	void onActivationChanged(ActivationState state) override;
	void onMouseMoved(float x, float y) override;
	void onMousePressed(float x, float y) override;
	void onMouseReleased(float x, float y) override;
	void onKeyPressed(const sf::Event::KeyChanged& key) override;
	void onKeyReleased(const sf::Event::KeyChanged& key) override;

	void centerText();
	void press();
	void release();

	/*!!gfx::!!*/Text m_text;
	sf::Sprite m_background;
	bool m_pressed;

}; // class ImageButton

} // namespace sfw


#endif // _IODUIWEHDGJHDJKFHGNDJKHGNIUWHER8UIGH45_
