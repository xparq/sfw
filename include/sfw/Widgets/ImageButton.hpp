#ifndef GUI_IMAGEBUTTON_HPP
#define GUI_IMAGEBUTTON_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Elements/Text.hpp"

#include <string>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Font.hpp>

namespace sfw
{

/**
 * Push button linked to a sprite sheet
 */
class ImageButton: public Widget
{
public:
    ImageButton(const sf::Texture& texture, const std::string& label = "");

    ImageButton* setText(const std::string& label);
    std::string getText() const;

    ImageButton* setSize(sf::Vector2f size);

    ImageButton* setFont(const sf::Font& font);
    const sf::Font& getFont() const;

    ImageButton* setTextSize(size_t size);
    ImageButton* setTextStyle(sf::Text::Style style);
    ImageButton* setTextColor(sf::Color color);

    ImageButton* setTexture(const sf::Texture& texture);

    ImageButton* setCallback(std::function<void()> callback)         { return (ImageButton*) Widget::setCallback(callback); }
    ImageButton* setCallback(std::function<void(ImageButton*)> callback);

private:
    void draw(const gfx::RenderContext& ctx) const override;

    void onStateChanged(WidgetState state) override;
    void onMouseMoved(float x, float y) override;
    void onMousePressed(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onKeyReleased(const sf::Event::KeyEvent& key) override;

    void centerText();
    void press();
    void release();

    Text m_text;
    sf::Sprite m_background;
    bool m_pressed;
};

} // namespace

#endif // GUI_IMAGEBUTTON_HPP
