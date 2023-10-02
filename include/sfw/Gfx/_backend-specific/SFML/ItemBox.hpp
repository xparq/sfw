#ifndef _SFW_ITEMBOX_SFML_HPP_
#define _SFW_ITEMBOX_SFML_HPP_

#include "sfw/Gfx/Elements/Box.hpp"

#include <SFML/Graphics/Color.hpp>

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

    void setFillColor(sf::Color color);
    void setItemColor(sf::Color color);
    // This would draw a filled rect *over* the "item" of ItemBox, so `color`
    // would usually need an alpha value set accordingly to be practical:
    void setTintColor(sf::Color color);

    void applyState(ActivationState state);

    inline T& item() { return m_item; }
    inline const T& item() const { return m_item; }

private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;
    void onPress() override;
    void onRelease() override;

    T m_item;
    std::optional<sf::Color> m_itemColor;
    std::optional<sf::Color> m_tintColor;
};

} // namespace

#include "ItemBox.inl"

#endif // _SFW_ITEMBOX_SFML_HPP_
