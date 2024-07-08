//!! RENAME TO BoxedShape, or something (telling what that "Item" is)!

#ifndef _SEUORTY847S6Y87EWY8704MTOGS87TG7N896_
#define _SEUORTY847S6Y87EWY8704MTOGS87TG7N896_


#include "sfw/gfx/element/Box.hpp"
#include "sfw/gfx/element/Text.hpp"

#include "SAL/util/cpp/macros.hpp" // MIXIN()
#include "SAL/math/Vector.hpp"

#include <SFML/Graphics/Color.hpp>

#include <optional> // E.g. for optional coloring
#include <type_traits> // is_convertible_v


namespace sfw
{

template <class T>
concept Shape = requires(T shape, const gfx::RenderContext& rctx) {
	//!!... Not well-defined! Not even clear what the interface should be...
	//!! Some clients assume classes directly (calling sfw::Text::size(),
	//!! others assume sf::Shape, or just Transformable... But T should not even
	//!! have to be sf:: class, on the contrary!... So, TBD...
	//!!
	//!! Not even these names are stable, wrt. the evolving API (e.g. adapter/Text etc.);
	//!! neither sfw::Text nor sf::Text have these:
	//!!shape.getSize();
	//!!shape.setPosition();
	shape.draw(rctx);
};

/**
 * A Box with a gfx. element (passed as a template arg.) inside of it
 */
template <Shape T>
class ItemBox : SAL_MIXIN(Box)
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

	T&       item()       { return m_item; }
	const T& item() const { return m_item; }

	fVec2 contentSize() const {
		if constexpr (std::is_convertible_v<T, gfx::Text>) { return fVec2(m_item.size()); } //!! This should become part of the Shape concept though!
		//!!if constexpr (...Transfofmable?) { return fVec2(m_item.getSize().size); }
	} //!! Direct SFML API calls!

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
