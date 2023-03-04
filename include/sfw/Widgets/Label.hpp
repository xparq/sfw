#ifndef GUI_LABEL_HPP
#define GUI_LABEL_HPP

#include "sfw/Widget.hpp"

namespace sfw
{

/**
 * Widget for displaying a simple text
 * Passive widget: cannot be focused or trigger event
 */
class Label: public Widget
{
public:
    explicit Label(const sf::String& string = "");

    void setText(const sf::String& string);
    const sf::String& getText() const;

    void setFillColor(const sf::Color& color);
    const sf::Color& getFillColor() const;

    void setTextSize(size_t size);
    size_t getTextSize() const;

private:
    void draw(const gfx::RenderContext& ctx) const override;

    void recomputeGeometry() override;
    void onThemeChanged() override;

    sf::Text m_text;
};

} // namespace

#endif // GUI_LABEL_HPP
