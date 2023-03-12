#ifndef GUI_ITEMBOX_SFML_HPP
#define GUI_ITEMBOX_SFML_HPP

#include "sfw/Gfx/Shapes/Box.hpp"
#include <optional>

namespace sfw
{

/**
 * Utility class used by widgets for holding visual components
 */
template <class T>
class ItemBox: public Box
{
public:
    ItemBox(Box::Type type = Box::Click);
    ItemBox(const T& item, Box::Type type = Box::Click);

    void setItemColor(sf::Color color);

    void applyState(WidgetState state);

    inline T& item() { return m_item; }
    inline const T& item() const { return m_item; }

private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;
    void onPress() override;
    void onRelease() override;

    T m_item;
    std::optional<sf::Color> m_itemColor;
};

} // namespace

#include "ItemBox.inl"

#endif // GUI_ITEMBOX_SFML_HPP
