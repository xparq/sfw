//----------------------------------------------------------------------------
// 2c99b33@SFML removed the default sf::Text ctor with no font.
// This helper class can be used to help existing APIs continue to rely
// on the default font implicitly.
//----------------------------------------------------------------------------

#ifndef _TCC78TY678W78F49W678TNT62X438T6EW4T78CV456_
#define _TCC78TY678W78F49W678TNT62X438T6EW4T78CV456_


#include "SAL/gfx/element/Font.hpp"
#include "SAL/geometry/Rectangle.hpp"
#include "SAL/gfx/Color.hpp"
#include "SAL/gfx/Render.hpp"

#include "SAL/sfml.hpp" //!! Should be "interfaced" away!...

#include <SFML/Graphics/Text.hpp>

#include <string>
#include <string_view>


namespace SAL::gfx
{

class Text : public sf::Text, public SAL::gfx::Drawable
{
public:
	using native_type = sf::Text;

	Text(std::string_view s = "", unsigned int height = 30);

	void        set(std::string_view str);
	std::string get() const;

//!!	size_t length() const; //!!?? #432: NOT UTF8-aware! Returns the native "char length"!(?)
	bool   empty() const;

	//!! Could be done via sf::Transformable::setScale(), but no client code needs it yet, so... nope:
	//!!void  resize(fVec2 size) { return setSize(size); }

	fVec2 size() const; //!! Confusing! This is not the length of the content string!...

	fVec2 position() const;
	void  position(fVec2 pos);

	// Centering should do nothing with a null rect, but in fact we might use that internally
	// to recenter inside its "fake" bounding box, compensating for the default SFML misalignment.
	void  center_in(const geometry::fRect& box);

	//!! These would be nice for sparing the torture of dealing with the SFML text positioning quirk,
	//!! but, alas, getOrigin() can't lie, as long as our Text here is ALSO sf::Text... :-/
	//!!void  setOrigin(fVec2 o);
	//!!fVec2 getOrigin() const;

	void        font(const Font& font);
	const Font& font() const;

	void     font_size(unsigned s);
	unsigned font_size() const;

	void   color(Color c);
	Color  color() const;


	void draw(const gfx::RenderContext& ctx) const override;


	//--------------------------------------------------------------------
	// Internals...

protected:
	constexpr       auto& native()       { return *((      native_type*)this); }
	constexpr const auto& native() const { return *((const native_type*)this); }

private:
	void _recalc();

	//!! Should be automatic, controlled by a mode flag:
	void _realign(/*!!align_mode: pixel!!*/) { position(position().round()); }

	//--------------------------------------------------------------------
	// Data...

	const Font* _font_ptr;
	fVec2       _sf_padding;
};

} // namespace SAL::gfx


#endif // _TCC78TY678W78F49W678TNT62X438T6EW4T78CV456_
