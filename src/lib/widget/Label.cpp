#include "sfw/widget/Label.hpp"
#include "sfw/Theme.hpp"

#include "SAL/sfml.hpp" //!! Should be "interfaced" away!...

#include <SFML/Graphics/RenderTarget.hpp>

#include <string>


namespace sfw
{

Label::Label(std::string_view text)
{
	onThemeChanged(); //!!Calling it this way is a temp. kludge (for DRY). Also: it has to happen before the rest of the init.
	setFocusable(false);
	setText(text);
}


Label* Label::setText(std::string_view text)
{
	m_text.set(text);
	recomputeGeometry();
	return this;
}

std::string Label::getText() const
{
	return m_text.get();
}


Label* Label::setFillColor(const Color color)
{
	m_text.color(color);
	return this;
}

Color Label::getFillColor() const
{
	return m_text.color();
}


Label* Label::setFontSize(unsigned size)
{
	m_text.font_size(size);
	recomputeGeometry();
	return this;
}

unsigned Label::getFontSize() const
{
	return m_text.font_size();
}


Label* Label::setStyle(sf::Text::Style style)
{
	m_text.setStyle(style);
	return this;
}


void Label::draw(const gfx::RenderContext& ctx) const
{
	auto lctx = ctx;
	lctx.props.transform *= getTransform();
	m_text.draw(lctx);
}


void Label::onThemeChanged()
{
	m_text.font(Theme::getFont());
	m_text.position({Theme::PADDING, Theme::PADDING}); //!!??  ... + m_text.getLocalBounds().top});
	                                                   //!! Do the "canonical" SFML offest correction!... Preferably implicitly!
	m_text.color(Theme::click.textColor);
	m_text.font_size(Theme::fontSize);
	recomputeGeometry();
}


void Label::recomputeGeometry()
{
	setSize(m_text.size() + Theme::PADDING * 2);
}


} // namespace sfw
