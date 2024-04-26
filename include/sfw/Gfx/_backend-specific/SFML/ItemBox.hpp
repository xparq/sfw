#ifndef _SEUORTY847S6Y87EWY8704MTOGS87TG7N896_
#define _SEUORTY847S6Y87EWY8704MTOGS87TG7N896_

#include "sfw/Gfx/Elements/Box.hpp"

#include <SFML/Graphics/Color.hpp>

#include <optional>

namespace sfw
{

/**
 * A Box with a gfx. element (passed as a template arg.) inside of it
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

public: //! <- NOT private, because draw() may be accessed directly (statically),
        //!    rather than just polymorphically (dynamically) via a pointer!
	void draw(const gfx::RenderContext& ctx) const override;

private:
	void onPress() override;
	void onRelease() override;

	T m_item;
	std::optional<sf::Color> m_itemColor;
	std::optional<sf::Color> m_tintColor;
};

} // namespace sfw

#include "ItemBox.inl"

#endif // _SEUORTY847S6Y87EWY8704MTOGS87TG7N896_
