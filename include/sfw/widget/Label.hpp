#ifndef _D2IOLUEYU5WT7D2Y0F876GGY78FRTHGUWEF34G57_
#define _D2IOLUEYU5WT7D2Y0F876GGY78FRTHGUWEF34G57_


#include "sfw/Widget.hpp"
#include "sfw/gfx/element/Text.hpp"

#include <string_view>


namespace sfw
{

/**
 * Widget for displaying a simple text
 * Passive widget: can't react to inputs or trigger events
 */
class Label: public Widget
{
public:
    explicit Label(std::string_view text = "");

    Label* setText(std::string_view text);
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

}; // class Label

} // namespace sfw


#endif // _D2IOLUEYU5WT7D2Y0F876GGY78FRTHGUWEF34G57_
