#ifndef _TCC78TY678W78F49W678TNT62X438T6EW4T78CV456_
#define _TCC78TY678W78F49W678TNT62X438T6EW4T78CV456_


#include "sfw/gfx/element/Font.hpp"
#include "sfw/gfx/Render.hpp"
#include "sfw/Theme.hpp" // For the default font filename + storing the loaded font.
#include "sfw/geometry/Rectangle.hpp"
#include "sfw/adapter/sfml.hpp"

#include <SFML/Graphics/Text.hpp>

#include <string>
#include <string_view>


namespace sfw
{

class Text : public sf::Text, public gfx::Drawable
{
public:
	using native_type = sf::Text;

	Text(std::string_view s = "", unsigned int height = 30);

	void        set(std::string_view str);
	std::string get() const;

	//!! Could be done via sf::Transformable::setScale(), but no client code needs it yet, so... nope:
	//!!void  resize(fVec2 size) { return setSize(size); }

	fVec2 size() const;
	fVec2 position() const;

	void  reposition(fVec2 pos);

	// Centering should do nothing with a null rect, but in fact we might use that internally
	// to recenter inside its "fake" bounding box, compensating for the default SFML misalignment.
	void  center(const geometry::fRect& box);

	//!! These would be nice for sparing the torture of dealing with the SFML text positioning quirk,
	//!! but, alas, getOrigin() can't lie, as long as sfw::Text is ALSO sf::Text... :-/
	//!!void  setOrigin(fVec2 o);
	//!!fVec2 getOrigin() const;

	//!! Just using the original directly, with auto-converting Font -> sf::Font:
	//!! However, it WOULD need to be intercepted to auto-adjust for the weird SFML sizing quirk!
	//!! But this class can only set the font at construction-time, due to only being able to
	//!! store a reference to it!
	//!!void setFont(const Font&) { ...; _recalc(); }
	const Font& getFont() const { return _font_ref; }


	void draw(const gfx::RenderContext& ctx) const override;


	//--------------------------------------------------------------------
	// Internals...
private:
	void _recalc();

	//!! Should be automatic, controlled by a mode flag:
	void _realign(/*!!align_mode: pixel!!*/) { reposition(position().round()); }

	//--------------------------------------------------------------------
	// Data...

	const Font& _font_ref; //!! This being a ref. would prevent adding setFont(), BTW!... :-o
	fVec2       _sf_padding;
	fVec2       _pos_offset;
};

} // namespace sfw


#endif // _TCC78TY678W78F49W678TNT62X438T6EW4T78CV456_
