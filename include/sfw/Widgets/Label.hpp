#ifndef GUI_LABEL_HPP
#define GUI_LABEL_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Elements/Text.hpp"

#include <string>

namespace sfw
{

/**
 * Widget for displaying a simple text
 * Passive widget: cannot be focused or trigger event
 */
class Label: public Widget
{
public:
    explicit Label(const std::string& text = "");

    Label* setText(const std::string& text);
    std::string getText() const;

    Label* setFillColor(const sf::Color& color);
    const sf::Color& getFillColor() const;

    Label* setTextSize(size_t size);
    size_t getTextSize() const;

    Label* setStyle(sf::Text::Style style);

private:
    void draw(const gfx::RenderContext& ctx) const override;

    void recomputeGeometry() override;

    void onThemeChanged() override;

    Text m_text;
};

} // namespace

#endif // GUI_LABEL_HPP
