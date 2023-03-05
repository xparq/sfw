#ifndef GUI_SPRITEBUTTON_HPP
#define GUI_SPRITEBUTTON_HPP

#include "sfw/Widget.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace sfw
{

/**
 * Push button linked to a sprite sheet
 */
class SpriteButton: public Widget
{
public:
    SpriteButton(const sf::Texture& texture, const sf::String& label = "");

    SpriteButton* setString(const sf::String& string);
    const sf::String& getString() const;

    SpriteButton* setFont(const sf::Font& font);
    const sf::Font& getFont() const;

    SpriteButton* setTextSize(size_t size);

    SpriteButton* setTexture(const sf::Texture& texture);

    SpriteButton* setCallback(std::function<void()> callback)         { return (SpriteButton*) Widget::setCallback(callback); }
    SpriteButton* setCallback(std::function<void(SpriteButton*)> callback);

private:
    void draw(const gfx::RenderContext& ctx) const override;

    void onStateChanged(WidgetState state) override;
    void onMouseMoved(float x, float y) override;
    void onMousePressed(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onKeyReleased(const sf::Event::KeyEvent& key) override;

    void press();
    void release();

    sf::Text m_text;
    sf::Sprite m_background;
    bool m_pressed;
};

} // namespace

#endif // GUI_SPRITEBUTTON_HPP
