#include "sfw/Widgets/Label.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <iostream>
using namespace std;

namespace sfw
{

Label::Label(const sf::String& string)
{
    onThemeChanged(); //!!This kludge will not be needed when the init proc. is reworked...

    setSelectable(false);
    setText(string);
}


void Label::setText(const sf::String& string)
{
    m_text.setString(string);
    recomputeGeometry();
}


const sf::String& Label::getText() const
{
    return m_text.getString();
}


void Label::setFillColor(const sf::Color& color)
{
    m_text.setFillColor(color);
}


const sf::Color& Label::getFillColor() const
{
    return m_text.getFillColor();
}


void Label::setTextSize(size_t size)
{
    m_text.setCharacterSize((unsigned)size);
    recomputeGeometry();
}


size_t Label::getTextSize() const
{
    return m_text.getCharacterSize();
}


void Label::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_text, sfml_renderstates);
#ifdef DEBUG
//    Widget::draw_outline({ctx.target, sfml_renderstates}); // Not the original, untransformed ctx.props!
#endif
}


void Label::onThemeChanged()
{
cerr << "Label" << endl;//!!
    m_text.setFont(Theme::getFont());
    m_text.setPosition({Theme::PADDING, Theme::PADDING});
    m_text.setFillColor(Theme::click.textColor);
    m_text.setCharacterSize((unsigned)Theme::textSize);
}


void Label::recomputeGeometry()
{
    Widget::setSize(
        m_text.getLocalBounds().width + Theme::PADDING * 2, m_text.getLocalBounds().height + Theme::PADDING * 2
    );
}

} // namespace
