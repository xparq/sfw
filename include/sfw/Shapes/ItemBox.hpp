#ifndef GUI_ITEMBOX_HPP
#define GUI_ITEMBOX_HPP

#include "sfw/Shapes/Box.hpp"

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

    void applyState(WidgetState state);

    inline T& item() { return m_item; }
    inline const T& item() const { return m_item; }

private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;
    void onPress() override;
    void onRelease() override;

    T m_item;
};

} // namespace

#include "ItemBox.inl"

#endif // GUI_ITEMBOX_HPP
