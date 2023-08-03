#ifndef SFW_GEOMETRY_HPP
#define SFW_GEOMETRY_HPP

#include "sfw/util/generic.hpp" // using sfw::Unknown, sfw::UseDefault;

namespace sfw
{
	/*
	2D (on-screen) orientation of geometric entities

	- The interpretation of Horizontal|Vertical combinations as "diagonal"
	  is intentionally undefined here.)
	*/
	struct Orientation // `enum class Orientation` would forbid all the num. ops. (despite :int)!
	//!! This is the best trade-off between the namespace pollution of plain
	//!! enums and the overly restrictive & cumbersome enum classes. This way
	//!! there's no mandatory `Orientation::` qualifiers, but only for selected
	//!! names, where that makes sense. Also, implicit num. conversions work.
	{
		enum _symbolic_ : int
		{
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#endif
		//!! Note: an extra `Param::` for the sfw:: ones was needed to work around a
		//!! CLANG-14 crash I used to have for a while; see more in util/generic.hpp!
		Unknown = sfw::Unknown,
		UseDefault = sfw::UseDefault,
			// `Default` would be hopelessly ambiguous: should it directly
			// encode a value (like `Horizontal`), and then remain hardcoded
			// + possibly too context-specific forever, or should it have a
			// dedicated pseudo-value meaning "use the actually configured
			// default for the given usage context"?...
#ifdef __clang__
#pragma clang diagnostic pop
#endif

		Horizontal = 1,
		Vertical = Horizontal << 1, // Support Horizontal|Vertical

		Directed = 1 << 4,
			Leftward  = Directed | Horizontal,
			Rightward = Directed | Horizontal | (Directed << 1),
			Upward    = Directed | Vertical,
			Downward  = Directed | Vertical   | (Directed << 1),
		} value;

		Orientation() : value(Unknown) {}
		Orientation(_symbolic_ x) : value(x) {}

		operator _symbolic_() const { return value; }
		//!!Unfortunately, this doesn't alleviate the need for an op= and op!=:
		operator int()        { return value; }
		bool operator ==(_symbolic_ x) const { return x == value; }
		bool operator !=(_symbolic_ x) const { return x != value; }
	};

	// "Export" symbols for unqualified use at the namespace level...
	// (I.e. emulating (only the good parts of) plain enums.
	// Note: an enum class can't be `using`ed into scope all at once,
	// only its values one by one! :-/ )
	using Orientation::Horizontal, Orientation::Vertical,
	      //Orientation::Directed, // <- This one's kinda internal.
	      Orientation::Leftward, Orientation::Rightward, Orientation::Upward, Orientation::Downward;

	static_assert(Orientation::Unknown == (int)sfw::Unknown);
	static_assert(Orientation::UseDefault == (int)sfw::UseDefault);
	static_assert(Orientation::Unknown != Orientation::Horizontal);
	static_assert(Orientation::UseDefault != Orientation::Horizontal);
	static_assert(Orientation::Directed > (Orientation::Horizontal|Orientation::Vertical));

} // namespace

#endif // SFW_GEOMETRY_HPP
