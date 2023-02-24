#include "sfw/SpriteButton.hpp"
#include "sfw/Theme.hpp"

namespace gui
{

SpriteButton::SpriteButton(const sf::Texture& texture, const sf::String& string):
    Widget(),
    m_pressed(false)
{
    int width = texture.getSize().x;
    int height = texture.getSize().y / 3; // default, hover, focus

    m_background.setTexture(texture);
    m_background.setTextureRect(sf::IntRect({0, 0}, {width, height}));

    setSize((float)width, (float)height);

    m_text.setFont(Theme::getFont());
    m_text.setCharacterSize((unsigned)Theme::textSize);

    setString(string);
}


void SpriteButton::setString(const sf::String& string)
{
    m_text.setString(string);
    centerText(m_text);
}


const sf::String& SpriteButton::getString() const
{
    return m_text.getString();
}


void SpriteButton::setFont(const sf::Font& font)
{
    m_text.setFont(font);
    centerText(m_text);
}


const sf::Font& SpriteButton::getFont() const
{
    return *m_text.getFont();
}


void SpriteButton::setTextSize(size_t size)
{
    m_text.setCharacterSize((unsigned)size);
    centerText(m_text);
}


void SpriteButton::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    auto lstates = states;
    lstates.transform *= getTransform();
    target.draw(m_background, lstates);
    target.draw(m_text, lstates);
}


// Callbacks -------------------------------------------------------------------

void SpriteButton::onStateChanged(State state)
{
    sf::Vector2i size{(int)getSize().x, (int)getSize().y};
    switch (state)
    {
    case StateDefault:
        m_background.setTextureRect(sf::IntRect({0, 0}, {size.x, size.y}));
        break;
    case StateHovered:
        m_background.setTextureRect(sf::IntRect({0, size.y}, {size.x, size.y}));
        break;
    case StatePressed:
    case StateFocused:
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

}
