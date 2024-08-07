#include "SAL/gfx/element/Text.hpp"
#include "SAL/util/diagnostics.hpp"

#include "sfw/Theme.hpp" //!!!!!!!!!!! NO!... Don't depend on it! But the mandatory default font for SFML... :-( 

#include <cassert>


//--------------------------------------------------------------------
// Draw a border around the text, to check positioning & sizing
// (See using it in Text::draw()!)
#ifdef DEBUG	
#  include <SFML/Graphics/RectangleShape.hpp>
#  include "sfw/gfx/Color.hpp"
#  include <string>
namespace
{
	using namespace std;
	using namespace SAL/*!!::math!!*/; using namespace SAL::gfx; using namespace SAL::geometry;
	void draw_border(const gfx::RenderContext& ctx, const Text* parent, Color bordercolor, fVec2 sfml_offset = fVec2(0,0))
	{
		[[maybe_unused]] auto ODUMP = [](const Text* t, string s, sf::RectangleShape* r) { if (t->get() == s) {
			cerr << "===>>  Origin & pos. of '"<<s<<"': " << fVec2(t->getOrigin()) << "; " << fVec2(t->getPosition())
			     <<    "   (O./pos of border: " << fVec2(r->getOrigin()) << "; " << fVec2(r->getPosition()) << ")"
			     << '\n';
			} };

		auto r = sf::RectangleShape(parent->size());
			// If the origin is set properly, positioning, rotation and scaling will all work:
			r.setOrigin(fVec2(parent->getOrigin()) - sfml_offset);
			r.setScale(parent->getScale());
			r.setRotation(parent->getRotation());
			r.setPosition(fVec2(parent->position()));
			// But if the origin can't be determined, that would throw everything off...
			// I leave this failed attempt here as a memento...:
			//r.setPosition(fVec2(parent->position()) + fVec2(parent->getScale().x * parent->_sf_padding.x(), parent->getScale().y * parent->_sf_padding.y() )); //!!??
				//!!?? The frames are still not always properly aligned...
				//!!?? Rotation puts it off completely (-> the raw SFML canvas test widget), and the ImageButtons are also wrong,
				//!!?? as well as the Button labels in general, and Label texts, too (the same way as buttons)! :-o
				//!!?? Also the progressbar percent captions are offset, too.
				//!!??
				//!!?? Also: why exaclty do I need to scale the offset, but not the position, again?!
				//!!   -> Because the scaling is relative to the origin, so if it's not 0, then the
				//!!      origin offset will translate to a scaled displacement!
				//!!??
			r.setFillColor(Color::None);
			r.setOutlineColor(bordercolor);
			r.setOutlineThickness(1);
		ctx.target.draw(r, ctx.props);
//ODUMP(parent, "All defaults", &r);
//ODUMP(parent, "Hello world!", &r);
// Recolored redraw, to dbl-check (just while pressing Alt, or whatever triggers this function):
//sf::Text t(*parent); t.setFillColor(Color::Cyan); ctx.target.draw(t, ctx.props);
	}
}
#endif // DEBUG
//--------------------------------------------------------------------


//============================================================================
namespace SAL::gfx
{
//using std::string, std::string_view;


Text::Text(std::string_view s/* = ""*/, unsigned int height/* = 30*/) :
	native_type(sfw::Theme::getFont().native(), SAL::stdstringview_to_SFMLString(s), height),
//!!?? Rethink (see e.g. `Font& font()`!...):
	_font_ptr(&sfw::Theme::getFont()) // Mirroring that of sf::Text, solely for use within the wrapper layer!
{
	_recalc();
}


//----------------------------------------------------------------------------
void Text::set(std::string_view str)
{
	native().setString(SAL::stdstringview_to_SFMLString(str));
	_recalc();
}

std::string Text::get() const
{
	return SAL::SFMLString_to_stdstring(native().getString());
}

/*!!??
size_t Text::length() const
{
	return native().getString().getSize(); //!!?? #432: NOT UTF8-aware!
}
??!!*/

bool Text::empty() const
{
	return native().getString().isEmpty();
}

//----------------------------------------------------------------------------
fVec2 Text::size() const
{
#if 0/*disabled*/ && defined(DEBUG) && defined(SAL_VECTOR_STREAMABLE)
cerr << "SFML px offset of ["<<get()<<"]: " << fVec2(getLocalBounds().position)
          << "   ---   O: "                      << fVec2(getOrigin())
          << "   ---   size: "                   << fVec2(getLocalBounds().size)
	  << '\n';
#endif
	return fVec2(getLocalBounds().size);
	 	//!! Note: this is usually SMALLER than the ACTUAL size of the sf::Text rect.! :-(
		//!!       (The bounding rect is offset a few pixels down from the top, as an inferior
		//!!       attempt at baselining adjacent text objects.)
		//!! Also: compilers may fail to optimise away the temporary!
}

// Centering should do nothing with a null rect, but in fact we might use that internally
// to recenter inside its "fake" bounding box, compensating for the default SFML misalignment.
void  Text::center_in(const geometry::fRect& box) //!! Can't get myself making this also a template on Rect's NumT...
{
//!! No, can't just temporarily set it: setScale and setRotation etc. don't render
//!! anything, they just set parameters with effects deferred to draw()ing! :)
//!!	auto save = getOrigin();

	// Rounding is critical here, after both divisions, to avoid blurred rendering:
	setOrigin( ((size() / 2) + _sf_padding).round() );
	position( (box.position() + (box.size() / 2)).round());
/*!!
#ifdef DEBUG
cerr	<< "BOX pos: " << box.position()
	<< "	Text pos: " << position()
	<< "	Text O: " << fVec2(getOrigin())
	<< '\n';
#endif
!!*/
	//!!?? There's still an unexpected (1-pixel? or more?) offset at certain font sizes,
	//!!?? also manifesting as a displacement from the supposed text rect (shown in DEBUG
	//!!?? with LAlt), but it has nothing to do with the rounding here.

//!!	setOrigin(save);
}


//----------------------------------------------------------------------------
void  Text::position(fVec2 p)         {        native().setPosition(p); }
fVec2 Text::position() const          { return native().getPosition(); }


//----------------------------------------------------------------------------
void Text::font(const Font& f)
{
	assert(&f);
	native().setFont(f);
	_font_ptr = &f;
	_recalc();
}

const Font& Text::font() const
{
	assert(_font_ptr); //!!?? Just an assert is not enough here!...
		//!! OTOH: every font access must then run-time check it!... :-o
		//!! So, this assert SHOULD really be enough after all!

	// Does the native sf::Text have the same idea about the current font?
	[[maybe_unused]] const auto& sf_font = native().getFont(); // Unused in DEBUG builds!
	assert(&sf_font == &_font_ptr->native());

	return *_font_ptr;
}

void     Text::font_size(unsigned s)  { native().setCharacterSize((int)s); _recalc(); }
unsigned Text::font_size() const      { return native().getCharacterSize(); }


//----------------------------------------------------------------------------
void   Text::color(Color c) { native().setFillColor(c); }
Color  Text::color() const  { return native().getFillColor(); }


//----------------------------------------------------------------------------
void Text::_recalc() 
{
	_sf_padding = getLocalBounds().position;
	//!!??_pos_offset = _sf_padding / 2; //!!?? This is probably not the correct idea! (-> SFML#216)
	//!!?? Also, should it probably be rounded already here?
}


//--------------------------------------------------------------------
void Text::draw(const RenderContext& ctx) const //override
{
	ctx.target.draw((native_type&)*this, ctx.props);
#ifdef DEBUG	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt))
		draw_border(ctx, this, Color::Green, _sf_padding);
#endif
}


} // namespace SAL::gfx
