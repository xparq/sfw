#include "sfw/Widgets/ImageButton.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include <cmath>
    using std::round;

namespace sfw
{

ImageButton::ImageButton(const sf::Texture& texture, const sf::String& string):
    Widget(),
    m_pressed(false)
{
    setTexture(texture);

    m_text.setFont(Theme::getFont());
    m_text.setCharacterSize((unsigned)Theme::textSize);

    setString(string);
}


ImageButton* ImageButton::setTexture(const sf::Texture& texture)
{
    int width = texture.getSize().x;
    int height = texture.getSize().y / 3; // default, hover, focus

    m_background.setTexture(texture);
    m_background.setTextureRect(sf::IntRect({0, 0}, {width, height}));

    setSize((float)width, (float)height);
    return this;
}


ImageButton* ImageButton::setString(const sf::String& string)
{
    m_text.setString(string);
    centerText();
    return this;
}


const sf::String& ImageButton::getString() const
{
    return m_text.getString();
}


ImageButton* ImageButton::setFont(const sf::Font& font)
{
    m_text.setFont(font);
    centerText();
    return this;
}


const sf::Font& ImageButton::getFont() const
{
    return *m_text.getFont();
}


ImageButton* ImageButton::setTextSize(size_t size)
{
    m_text.setCharacterSize((unsigned)size);
    centerText();
    return this;
}


void ImageButton::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_background, sfml_renderstates);
    ctx.target.draw(m_text, sfml_renderstates);
}


// Callbacks -------------------------------------------------------------------

void ImageButton::onStateChanged(WidgetState state)
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


void ImageButton::onMouseMoved(float x, float y)
{
    if (isFocused())
    {
        if (containsPoint({x, y}) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            press();
        else
            release();
    }
}


void ImageButton::onMousePressed(float, float)
{
    press();
}


void ImageButton::onMouseReleased(float x, float y)
{

    release();
    if (containsPoint({x, y}))
    {
        triggerCallback();
    }
}


void ImageButton::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
    {
        triggerCallback();
        press();
    }
}


void ImageButton::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
        release();
}


void ImageButton::press()
{
    if (!m_pressed)
    {
        m_pressed = true;
        m_text.move({0, 1});
    }
}


void ImageButton::release()
{
    if (m_pressed)
    {
        m_pressed = false;
        m_text.move({0, -1});
    }
}


void ImageButton::centerText()
{
    auto [boxwidth, boxheight] = getSize();
    sf::FloatRect t = m_text.getLocalBounds();
    m_text.setOrigin({t.left + round(t.width / 2.f), t.top + round(t.height / 2.f)});
    m_text.setPosition({boxwidth / 2, boxheight / 2});
}


ImageButton* ImageButton::setCallback(std::function<void(ImageButton*)> callback)
{
    return (ImageButton*) Widget::setCallback( [callback] (Widget* w) { callback( (ImageButton*)w ); });
}

} // namespace
