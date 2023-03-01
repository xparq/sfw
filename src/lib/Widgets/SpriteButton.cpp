#include "sfw/Widgets/SpriteButton.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace gui
{

SpriteButton::SpriteButton(const sf::Texture& texture, const sf::String& string):
    Widget(),
    m_pressed(false)
{
    setTexture(texture);

    m_text.setFont(Theme::getFont());
    m_text.setCharacterSize((unsigned)Theme::textSize);

    setString(string);
}


SpriteButton* SpriteButton::setTexture(const sf::Texture& texture)
{
    int width = texture.getSize().x;
    int height = texture.getSize().y / 3; // default, hover, focus

    m_background.setTexture(texture);
    m_background.setTextureRect(sf::IntRect({0, 0}, {width, height}));

    setSize((float)width, (float)height);
    return this;
}


SpriteButton* SpriteButton::setString(const sf::String& string)
{
    m_text.setString(string);
    centerText(m_text);
    return this;
}


const sf::String& SpriteButton::getString() const
{
    return m_text.getString();
}


SpriteButton* SpriteButton::setFont(const sf::Font& font)
{
    m_text.setFont(font);
    centerText(m_text);
    return this;
}


const sf::Font& SpriteButton::getFont() const
{
    return *m_text.getFont();
}


SpriteButton* SpriteButton::setTextSize(size_t size)
{
    m_text.setCharacterSize((unsigned)size);
    centerText(m_text);
    return this;
}


void SpriteButton::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_background, sfml_renderstates);
    ctx.target.draw(m_text, sfml_renderstates);
}


// Callbacks -------------------------------------------------------------------

void SpriteButton::onStateChanged(WidgetState state)
{
    sf::Vector2i size({(int)getSize().x, (int)getSize().y});
    switch (state)
    {
    case WidgetState::Default:
        m_background.setTextureRect(sf::IntRect({0, 0}, {size.x, size.y}));
        break;
    case WidgetState::Hovered:
        m_background.setTextureRect(sf::IntRect({0, size.y}, {size.x, size.y}));
        break;
    case WidgetState::Pressed:
    case WidgetState::Focused:
        m_background.setTextureRect(sf::IntRect({0, size.y * 2}, {size.x, size.y}));
        break;
    }
}


void SpriteButton::onMouseMoved(float x, float y)
{
    if (isFocused())
    {
        if (containsPoint({x, y}) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            press();
        else
            release();
    }
}


void SpriteButton::onMousePressed(float, float)
{
    press();
}


void SpriteButton::onMouseReleased(float x, float y)
{

    release();
    if (containsPoint({x, y}))
    {
        triggerCallback();
    }
}


void SpriteButton::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
    {
        triggerCallback();
        press();
    }
}


void SpriteButton::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
        release();
}


void SpriteButton::press()
{
    if (!m_pressed)
    {
        m_pressed = true;
        m_text.move({0, 1});
    }
}


void SpriteButton::release()
{
    if (m_pressed)
    {
        m_pressed = false;
        m_text.move({0, -1});
    }
}


SpriteButton* SpriteButton::setCallback(std::function<void(SpriteButton*)> callback)
{
    return (SpriteButton*) Widget::setCallback( [callback] (Widget* w) { callback( (SpriteButton*)w ); });
}

} // namespace
