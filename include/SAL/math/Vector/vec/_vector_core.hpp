//============================================================================
// Vector class with replaceable data (via "backend" adapters)
//
// - No guaranteed direct property access (i.e. .x, ...): depends on the adapter!
// - The unified (but clunky) .get/.set/.ref accessors are always available.
//============================================================================

#ifndef _FDCCX46UJ08W746IUYJFT803RU78TWNF8W90RJT34DTFLGTH8MB_
#define _FDCCX46UJ08W746IUYJFT803RU78TWNF8W90RJT34DTFLGTH8MB_


#include "base.hpp"
#include "data-fw.hpp"

#ifdef VEC_CPP_IMPORT_STD
  import std;
#else
# include <utility> // integer_sequence, decay
#endif

#ifdef VEC_DEBUG
# include "io.hpp" // Can be disabled with VEC_NO_IOS, but still needed for the syntax!
#include "data.hpp" // VectorData for a static_assert...
# ifdef VEC_CPP_IMPORT_STD
   import std;
# else
#  include <iostream> // We're printing other things than vectors anyway...
# endif
#endif // VEC_DEBUG


#include "vector-fw.hpp" //! Including only for double-checking: should fail if out of sync!
namespace VEC_NAMESPACE {

//----------------------------------------------------------------------------
template<
	unsigned Dim,
	Scalar NumT, //! = float,
	template <unsigned, typename> class Adapter//! = VectorData
	                   //!!?? Using Scalar here would trigger a "...more constrained" error (Clang)! :-o
>
class Vector : public Adapter<Dim, NumT>
{
	//!! -> #33: An MSVC 17.x (CL 19.40) bug prevents doing these via `requires`:
	//#ifndef _MSC_VER //!! -> #33: An MSVC 17.x (CL 19.40) bug is triggered by this:
	//	requires (Dim >= 1) && VectorDataAdapter<Adapter<Dim, NumT>>
	//#endif
	static_assert(Dim >= 1);
	//!static_assert(VectorDataAdapter<Adapter<Dim, NumT>>);
	//! Moved the assert for Adapter to the ctor, to allow `if constexpr` on it!...

	using adapter_type = Adapter<Dim, NumT>;
	constexpr       adapter_type& adapter()       { return static_cast<      adapter_type&>(*this); }
	constexpr const adapter_type& adapter() const { return static_cast<const adapter_type&>(*this); }

public:
	using foreign_type = adapter_type::foreign_type; // Just for convenience (it might also need `typename` otherwise)!
	// UniformVectorData traits:
	constinit const static inline unsigned dim = Dim;
	using number_type = adapter_type::number_type;

	//!! These inherited methods are NOT considered by C++ without explicitly `using` them!...: :-/
	using adapter_type::get, adapter_type::set, adapter_type::ref;


	// Ctors...
	//

	// Reuse the adapter ctors:
	using adapter_type::adapter_type; //!!??C++: A perfect-forwarding ctor there might interfere with the local ctors here?

	// It's all up to the user; we're agnostic to this!
	// - Except that client code could break "{} is zero-init"! -> x1ab/flexivec#68
	// - And except that `Vector2 zero = {}` won't compile without a default ctor!... :-o (Why?!)
	constexpr Vector() = default;

	// Copy from another Vector:
	//--------------------------------------------------------------------
	//constexpr Vector(const Vector& other) = default;
#ifdef VEC_DEBUG
	constexpr Vector(const Vector& other) : adapter_type(other.adapter())
	{
std::cerr << "[Vector: COPY CTOR CALLED! other == "<< other <<"\n";
	}
	// And its op= pair:
	constexpr Vector& operator = (const Vector& other) = default;
#endif

	// Coord.-wise - implicit for same number types
	//--------------------------------------------------------------------
	template <Scalar... Coords>
		requires (sizeof...(Coords) == Dim && (std::same_as<Coords, number_type> && ...))
	constexpr Vector(Coords... coords) : adapter_type{coords...}
	{
#ifdef VEC_DEBUG
std::cerr << "[Vector: created from coordinates - with same NumT]\n";
#endif
	}

	// Coord.-wise - forced number conv. for diff. types
	//--------------------------------------------------------------------
	// (If the types aren't convertible, the compilation will fail.)
	template <Scalar... Coords>
		requires (sizeof...(Coords) == Dim && (!std::same_as<Coords, number_type> || ...))
		//!!?? Adding this didn't improve the comp. error of GCC *at all*: ... && (std::convertible_to<Coords, number_type> && ...) )
		//!!   (MSVC already does a fine job here on its own.)
#ifndef VEC_IMPLICIT_NUM_CONV
	explicit
#endif
	constexpr Vector(Coords... coords) : adapter_type{static_cast<number_type>(coords)...}
	{
#ifdef VEC_DEBUG
std::cerr << "[Vector: created from coordinates - with different NumT!]\n";
#endif
	}

	// Conv. from "vector-like" object - implicit with same number types
	//--------------------------------------------------------------------
	/*!!
	template <UniformVectorData V>
	constexpr Vector(const V& v)
		: adapter_type(v) //!! Well, but an adapter is not supposed to deal with anything like V!
		                  //!! So, instead, see the trick below, proxying to an also mandated,
				  //!! but much more natural and likely coord-wise ctor!
				  //!! (However, the concept-check for the presence of such a ctor is disabled now; see #34!)
	{} !!*/
	template <UniformVectorData V>
		requires (V::dim == Dim && std::same_as<typename V::number_type, NumT>)
	constexpr Vector(const V& v)
		: Vector(v, std::make_integer_sequence<unsigned, Dim>{}) {
#ifdef VEC_DEBUG
std::cerr << "[Vector: converted from UniformVectorData with same NumT]\n";
#endif
	}
		private: template <UniformVectorData V, unsigned... Indices>
			requires (V::dim == Dim && std::same_as<typename V::number_type, NumT>)
		explicit constexpr Vector(const V& v, std::integer_sequence<unsigned, Indices...>)
			//!! Can't just conjure up some "canonical form" of a data-native ctor, so we're forced to
			//!! use the coordinate-wise one arbitrarily required by the UniformVectorData concept:
			: adapter_type{v.template get<Indices>()...} {} //!! Optimize to use ref() for big enough NumT sizes!
				//!! Brace-init may still work even if the adapter (array!) has no such ctor!
		public:

	// Conv. from "vector-like" object - forced number conv. for diff. num. types
	// - explicit, unless VEC_IMPLICIT_NUM_CONV
	//--------------------------------------------------------------------
	template <UniformVectorData V>
		requires (V::dim == Dim && !std::same_as<typename V::number_type, NumT>) //!!?? && std::is_nothrow_convertible<V::number_type, NumT>)
#ifndef VEC_IMPLICIT_NUM_CONV
	explicit
#endif
	constexpr Vector(const V& v)
		: Vector(v, std::make_integer_sequence<unsigned, Dim>{}) {
#ifdef VEC_DEBUG
std::cerr << "[Vector: converted from UniformVectorData - with different NumT!]\n";
#endif
	}
		private: template <UniformVectorData V, unsigned... Indices>
			requires (V::dim == Dim && !std::same_as<typename V::number_type, NumT>)
		explicit constexpr Vector(const V& v, std::integer_sequence<unsigned, Indices...>)
			: adapter_type{static_cast<NumT>(v.template get<Indices>())...} {}
				//!! Brace-init may still work even if the adapter (array!) has no such ctor!
		public:


	// Conv. from "vector-coord.-struct-like" object - same number-type
	//--------------------------------------------------------------------
	//!! Unfortunately, these will suppress the `explicit` modifier of the target ctors
	//!! (if VEC_IMPLICIT_NUM_CONV is not defined)! :-/
	template <VectorDirectCoords<Dim, NumT> V> requires (Dim == 2)
	constexpr Vector(const V& v) : Vector(v.x, v.y) {}

	template <VectorDirectCoords<Dim, NumT> V> requires (Dim == 3)
	constexpr Vector(const V& v) : Vector(v.x, v.y, v.z) {}

	template <VectorDirectCoords<Dim, NumT> V> requires (Dim == 4)
	constexpr Vector(const V& v) : Vector(v.x, v.y, v.z, v.w) {}

/*!!
	// Convert from adapter_type
	//--------------------------------------------------------------------
	constexpr Vector(const adapter_type& dadapter) //!!?? -> templatize for A&& and std::forward<decltype(a)>(...)?
		: adapter_type(dadapter) //!! Optimize to use ref...() for big enough NumT sizes!
	{
#ifdef VEC_DEBUG
std::cerr << "[Vector: CTOR with DataAdapter == "<< dadapter <<"]\n";
#endif
	}

	// And its op= pair:
	constexpr Vector& operator = (const adapter_type& dadapter) //!!?? -> templatize for A&& and std::forward<decltype(a)>(...)?
	{
//!!		(adapter_type&)*this = dadapter; //!! This would implicitly require op= at least defaulted in every adapter! :-(

		[this]<unsigned... I>(const auto& vdata, std::integer_sequence<unsigned, I...>)
		constexpr { ((this->template ref<I>() = vdata.template get<I>()), ...);
		} (dadapter, std::make_integer_sequence<unsigned, dim>{});

		return *this;
	}
!!*/

	// Convert from the underlying ("foreign") data type
	//--------------------------------------------------------------------
	constexpr Vector(const foreign_type& data)
		: adapter_type(data)
	{
#ifdef VEC_DEBUG
std::cerr << "[Vector: CTOR with opaque foreign data (which we can't print...)]\n";
#endif
	}

	// And its op= pair (-> #20):
	constexpr Vector& operator = (const foreign_type& data)
	{
#ifdef VEC_DEBUG
std::cerr << "[Vector: op= with opaque foreign data (which we can't print...)]\n";
#endif
		(adapter_type&)*this = data; //!! "implicitly-declared 'adapter...::operator=(const adapter...&)' is deprecated"
		                             //!!?? (adapter_type&) = ... implies at least a defaulted op= in every adapter (-- why exactly??!!)
//		(foreign_type&)*this = data; //!! OTOH, this silently assumes that adapters can't have any state (data)! :-/
/*!!
		[this]<unsigned... I>(const auto& vdata, std::integer_sequence<unsigned, I...>)
		constexpr { ((this->template ref<I>() = vdata.template get<I>()), ...);
		} (dadapter, std::make_integer_sequence<unsigned, dim>{});
!!*/
		return *this;
	}


	// Convert to the underlying data type (via its adapter):
	//--------------------------------------------------------------------
	constexpr operator       foreign_type& ()       { return (      foreign_type&)adapter(); }
	constexpr operator const foreign_type& () const { return (const foreign_type&)adapter(); }


	//--------------------------------------------------------------------
	// Vector Operations...
	//--------------------------------------------------------------------

	constexpr operator bool () const;

	constexpr Vector& operator += (const Vector& other_v);
	constexpr Vector& operator -= (const Vector& other_v);
	constexpr Vector& operator *= (const Vector& other_v);
	constexpr Vector& operator /= (const Vector& other_v);

	constexpr Vector& operator += (number_type n);
	constexpr Vector& operator -= (number_type n);
	constexpr Vector& operator *= (number_type n);
	constexpr Vector& operator /= (number_type n);

	constexpr auto length() const;
	constexpr auto length_squared() const;

	constexpr Vector& normalize(); // -> #11
	constexpr Vector  normalized() const; // -> #11

	// Often needed e.g. to avoid subpixel AA-blurring with OpenGL (noop for integer types):
	constexpr Vector& round();
	constexpr Vector  rounded() const;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - -
#ifdef VEC_NO_DIRECT_COORDS //!! -> #48
//!! No way to actually remove members from the overload set by SFINAE or concepts
//!! (like ...requires (!HasDirectCoords<adapter_type>) etc.) Those happen too late:
//!! the name is already introduced, and will stick; it would just get disabled
//!! (if my (and ChatGPT's) current understanding is correct).
	// Getters:
	constexpr NumT  x()  const                     { return adapter().template get<0>(); }
	constexpr NumT  y()  const                     { return adapter().template get<1>(); }
	constexpr NumT  z()  const  requires(Dim >= 3) { return adapter().template get<2>(); }
	constexpr NumT  w()  const  requires(Dim >= 4) { return adapter().template get<3>(); }
	// Setters:
	constexpr auto& x(NumT val)                    { adapter().template set<0>(val); return *this; }
	constexpr auto& y(NumT val)                    { adapter().template set<1>(val); return *this; }
	constexpr auto& z(NumT val) requires(Dim >= 3) { adapter().template set<2>(val); return *this; }
	constexpr auto& w(NumT val) requires(Dim >= 4) { adapter().template set<3>(val); return *this; }
#endif // VEC_NO_DIRECT_COORDS

}; // class Vector


// We also need an entry for Vector in Vector_traits. (-> #19)
//!!?? FORGOT: WHY BOTH THIS *AND* ALSO REPEATING IT INSIDE THE CLASS?!
//!!?? ALSO: WHY CAN THIS ONE COME AFTER THE CLASS, BUT COULDN'T WITH THE OLD Vector?
//----------------------------------------------------------------------------
template <unsigned Dim, Scalar NumT, template <unsigned, typename> class Adapter>
struct Vector_traits< Vector<Dim, NumT, Adapter> > {
	static constinit const inline unsigned dim = Dim;
	using number_type = NumT;
};


// 'we good?
#ifdef VEC_DEBUG
static_assert(AdapterVector<Vector</*dummy*/3, /*dummy*/float, VectorData>>);
#endif


// Aliases for dims. 2-4, and/or the native VectorData "backend"
//----------------------------------------------------------------------------
#define _VEC_DEF_ template <Scalar NumT = float, template <unsigned, typename> class Adapter = VectorData>
  _VEC_DEF_ using Vector2 = Vector<2, NumT, Adapter>;
  _VEC_DEF_ using Vector3 = Vector<3, NumT, Adapter>;
  _VEC_DEF_ using Vector4 = Vector<4, NumT, Adapter>;
#undef _VEC_DEF_


// Deduction guides for dims. 2-4 (higher dims. need explicit tagging!)
//----------------------------------------------------------------------------
#define _VEC_DEF_ template <Scalar NumT = float, template <unsigned, Scalar> class Adapter = VectorData>
  _VEC_DEF_ Vector(NumT x, NumT y)                 -> Vector<2, NumT, Adapter>;
  _VEC_DEF_ Vector(NumT x, NumT y, NumT z)         -> Vector<3, NumT, Adapter>;
  _VEC_DEF_ Vector(NumT x, NumT y, NumT z, NumT w) -> Vector<4, NumT, Adapter>;
#undef _VEC_DEF_


//----------------------------------------------------------------------------
// Vector Operations -- free functions
//----------------------------------------------------------------------------

// Helpers...
//----------------------------------------------------------------------------
template <class V1, class V2> //!! "Concepts can't be constrained", so NOT: template <UniformVectorData V1, UniformVectorData V2>
concept VectorData_SameDim = (V1::dim == V2::dim);

// Dear Preprocessor, thanks for kinda saving the day from even more C++ horror!....
#define _VEC_STANDALONE_BIVECT_FN_(Name) \
	template <UniformVectorData V1, UniformVectorData V2> requires VectorData_SameDim<V1, V2> \
	constexpr auto Name (const V1& v1, const V2& v2)
// Mixed vector vs. scalar op. helpers:
#define _VEC_STANDALONE_VECT_SCALAR_FN_(Name) template <UniformVectorData V, Scalar N> \
	constexpr auto Name (const V& v, N n)
#define _VEC_STANDALONE_SCALAR_VECT_FN_(Name) template <UniformVectorData V, Scalar N> \
	constexpr auto Name (N n, const V& v)

//----------------------------------------------------------------------------
_VEC_STANDALONE_BIVECT_FN_(operator ==);

// Pairwise ops...
_VEC_STANDALONE_BIVECT_FN_(operator +);
_VEC_STANDALONE_BIVECT_FN_(operator -);
_VEC_STANDALONE_BIVECT_FN_(operator *);
_VEC_STANDALONE_BIVECT_FN_(operator /);

_VEC_STANDALONE_BIVECT_FN_(dot);

// Mixed vector vs. scalar ops....
_VEC_STANDALONE_VECT_SCALAR_FN_(operator +);
_VEC_STANDALONE_VECT_SCALAR_FN_(operator *);
// Also in reverse, for commutativity:
_VEC_STANDALONE_SCALAR_VECT_FN_(operator +);
_VEC_STANDALONE_SCALAR_VECT_FN_(operator *);
// These are not commutative:
_VEC_STANDALONE_VECT_SCALAR_FN_(operator -);
_VEC_STANDALONE_VECT_SCALAR_FN_(operator /);

} // namespace VEC_NAMESPACE


//============================================================================
// IMPL...
//============================================================================
#include "_vector_core.inl"

// Still needed by ops.inl above:
#undef _VEC_STANDALONE_SCALAR_VECT_FN_
#undef _VEC_STANDALONE_VECT_SCALAR_FN_
#undef _VEC_STANDALONE_BIVECT_FN_


#endif // _FDCCX46UJ08W746IUYJFT803RU78TWNF8W90RJT34DTFLGTH8MB_
