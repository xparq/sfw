#ifndef GUI_LABEL_HPP
#define GUI_LABEL_HPP

#include "sfw/Widget.hpp"

#include <string>

#include <SFML/Graphics/Text.hpp>

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

    void setText(const std::string& text);
    std::string getText() const;

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
