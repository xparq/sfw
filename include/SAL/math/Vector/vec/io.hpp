#ifndef _XMCOITY5897MWCED8097R6Y87FMVYO87IER6Y8NHE56I68GH_
#define _XMCOITY5897MWCED8097R6Y87FMVYO87IER6Y8NHE56I68GH_

/*===========================================================================
  Stream ops.

  NOTES:

    -	`operator >>` doesn't throw a runtime_error exception for invalid
	input format, but sets std::ios::failbit instead (to be checked with
	`in.fail()`).

    -	Separators can only be single letters, too, but io::separator can
	also contain trailing whitespace -- which will always be written,
	but will always be skipped (rather than checked) upon reading.

	Separators may also be just whitespece entirely.

    -	Only single-letter, or empty (""), open/close brackets are supported,
	but the opening bracket may have trailing, and the closing bracket
	may have leading whitespace.

	(If set to empty (""), insert whitespace between multiple streamed
	vectors to still allow reading them back. Alternatively, setting the
	open bracket to empty, but setting the closing one to non-empty AND
	NOT whitespace(!) would still work for readback.)

 ----------------------------------------------------------------------------
  TIP:
	<iostream> is heavy, so only use this if you're really going to write
	(or read) vectors (or you've already included <iostream> anyway).

	You can also #define VEC_NO_IOS (before including this header) to
	disable IO. (Any `<< myvec` expressions will stay valid, just get
	compiled to nothing, and <iostream> will NOT be included either.)

 ===========================================================================*/


#ifndef VEC_IO_SEPARATOR_DEFAULT
#define VEC_IO_SEPARATOR_DEFAULT ", "
#endif
#ifndef VEC_IO_BRACKET_OPEN_DEFAULT
#define VEC_IO_BRACKET_OPEN_DEFAULT "("
#endif
#ifndef VEC_IO_BRACKET_CLOSE_DEFAULT
#define VEC_IO_BRACKET_CLOSE_DEFAULT ")"
#endif

#include "base.hpp"


// - - - - - - - - - - - - - - - - - - - - - -
#ifdef VEC_NO_IOS
// - - - - - - - - - - - - - - - - - - - - - -

#	ifdef VEC_CPP_IMPORT_STD
	 import std;
#	else
#	 include <iosfwd>
#	endif

  namespace VEC_NAMESPACE {
	namespace io {
		const char* io::separator     = "";
		const char* io::bracket_open  = "";
		const char* io::bracket_close = "";
	}

	template <UniformVectorData V> std::ostream& operator << (std::ostream& out, const V&) { return out; }
	template <UniformVectorData V> std::istream& operator >> (std::istream& in, V&) { return in; }

  } // namespace VEC_NAMESPACE

// - - - - - - - - - - - - - - - - - - - - - -
#else // VEC_NO_IOS
// - - - - - - - - - - - - - - - - - - - - - -

#	ifdef VEC_CPP_IMPORT_STD
	 import std;
#	else
#	 include <iostream>
#	 include <cctype> // isspace
#	endif
	//!!
	//!!#include <iosfwd> would only be meaningful if the op<</>> impl. could move to a separate transl. unit,
	//!! not just to an .inl! But it can't.
	//!! Anyway, after fixing the concepts, move the impl. to the .inl regardless!
	//!!

  namespace VEC_NAMESPACE {
	namespace io {
		const char* separator     = VEC_IO_SEPARATOR_DEFAULT;
		const char* bracket_open  = VEC_IO_BRACKET_OPEN_DEFAULT;
		const char* bracket_close = VEC_IO_BRACKET_CLOSE_DEFAULT;
	}

	template <UniformVectorData V>
	std::ostream& operator << (std::ostream& out, const V& v)
	{
		assert(V::dim >= 1);

		//!! There's a comp-time way to unroll this loop, but I'm not willing to accept the lack of a "static" std::for_each! ;)
		out << io::bracket_open << v.get(0); //!!?? WTF: get(0) NOT FOUND WITHOUT PROXIES IN Vector?!
		for (unsigned i = 1; i < V::dim; ++i)
			out << io::separator << v.get(i);
		out << io::bracket_close;
		return out;
	}

	template <UniformVectorData V>
	std::istream& operator >> (std::istream& in, V& v)
	{
		// Remember the 1st non-whitespace pos of the closing bracket:
		unsigned br_close_nonspace_pos = [](const char* token) { //! Not static: the tokens can change between >> calls!
			unsigned pos = 0;
			if (token) while (std::isspace(token[pos])) ++pos;
			return pos;
		} (io::bracket_open[0] ? io::bracket_close : io::bracket_open); // DON'T skip whitespace if the open br. is empty!

		assert(V::dim >= 1);
		typename V::number_type val;
		char ch;
		if (io::bracket_open && io::bracket_open[0]) {
			in >> ch;
//std::cerr <<" ["<<ch<<"]";
			if (ch != io::bracket_open[0]) { in.setstate(std::ios::failbit); return in; }
		}

		in >> std::ws >> val; //!! Err. handl.!
		in >> std::noskipws;

		//!! There's a comp-time way to unroll this loop, but I'm not willing to accept the lack of a "static" std::for_each! ;)
		v.set(0, val);
		for (unsigned i = 1; i < V::dim; ++i) {
			in >> ch; // Separator...
//std::cerr <<" [OK/1? "<<!in.fail()<<"] ";
//std::cerr <<" ["<<ch<<"]";
			if (ch != io::separator[0]) { in.setstate(std::ios::failbit); return in; }
//std::cerr <<" [OK/2? "<<!in.fail()<<"] ";
			in >> std::ws >> val; //!! Err. handl.!
//std::cerr <<" [OK/3? "<<!in.fail()<<"] ";
			v.set(i, val);
		}
//std::cerr <<" [STILL OK/ENDLOOP? "<<!in.fail()<<"]\n";

		if (io::bracket_close && io::bracket_close[0]) {
			if (br_close_nonspace_pos) in >> std::ws; //! Only skip space the close bracket is known to have some!
			in >> ch;
//std::cerr <<" ["<<ch<<"]";
			if (ch != io::bracket_close[br_close_nonspace_pos]) { in.setstate(std::ios::failbit); return in; }
		}

		return in;
	}

  } // namespace VEC_NAMESPACE

// - - - - - - - - - - - - - - - - - - - - - -
#endif // VEC_NO_IOS
// - - - - - - - - - - - - - - - - - - - - - -


#endif // _XMCOITY5897MWCED8097R6Y87FMVYO87IER6Y8NHE56I68GH_
