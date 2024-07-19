//----------------------------------------------------------------------------
// Platform-independent vector type wrapper (the interface half of the adapter
// -- see the ..._Impl class for the implementation half, #included below)
//
// Define SAL_VECTOR_STREAMABLE to add << and >> (otherwise they are no-ops).
//
//----------------------------------------------------------------------------

#ifndef _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_
#define _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_


//!!--------------------------------------------------------------------------
//!!?? Move this to another header? (Used by both the adapter interface and impl.)
//!!   Possibly along with the definition of Scalar then (removing it from the
//!!   generic "util" stuff), making it part of this new set of concepts!
//!!

#include "Scalar.hpp"

#include <cmath> // round
#include <utility> // swap, forward
#include <type_traits> // is_same, enable_if


namespace SAL//!!::math
{
	template<class V>
	concept Vector = requires(V v) {
		V::dimensions;
		typename V::number_type;
		v.x(); v.y();
	};

	template <class V>
	concept SmallVector = (sizeof(V) <= 2 * sizeof(void*)); // For copy/by-ref optim.

} // namespace SAL//!!::math
//!!--------------------------------------------------------------------------


// The backend-specific adapter impl.:
#include "SAL.hpp"
#include SAL_ADAPTER(math/Vector)


namespace SAL//!!::math
{
	template <class Impl>
	class Vector_Interface : public Impl // mixin
	{
	public:
		// Convert from rval with different number type:
			//!! - These attempts both failed to enable this ctor for e.g. const iRect const_ir; auto cfr = fRect(const_ir4);
			//!!	template <class OtherT> Vector_Interface(OtherT&& other) ...
			//!!	... requires (!std::is_same_v<typename OtherT::number_type, typename Impl::number_type>)
		template <class OtherT,
			typename = std::enable_if_t< !std::is_same_v<typename std::decay_t<OtherT>::number_type, typename Impl::number_type> >
				//!! decay_t (or at least std::remove_cvref_t) is CRITICAL for accepting both const/non-const OtherT ctor args.! :-o
		>
		constexpr explicit Vector_Interface(OtherT&& other)
			: Impl( {(typename Impl::number_type)std::forward<OtherT>(other).x(),
			         (typename Impl::number_type)std::forward<OtherT>(other).y()} ) {}

		//!!?? How come this has never been actually called?! :-o
		template <class SameT,
			typename = std::enable_if_t< std::is_same_v<typename std::decay_t<SameT>::number_type, typename Impl::number_type> >
				//!! decay_t (or at least std::remove_cvref_t) is CRITICAL for accepting both const/non-const OtherT ctor args.! :-o
		>
		constexpr Vector_Interface(const SameT& other) : Impl(std::forward(other.native()))
		{ //!!??static_
			assert("FINALLY HERE!... :)" && false); } //!!?? WHY IS THIS NEVER ACTUALLY INSTANTIATED IN GCC, BUT IS IN MSVC?! :-o (No failed static_assert in GCC! :-o )


		// Convert from native (backend) vector:
		using Impl::Impl; // Elevate the backend-specific ctors for implicit backend -> adapter conversions!

		// Convert to the native type:
		constexpr operator const typename Impl::native_type& () const { return Impl::native(); } //!!?? Should just return by-val, as the other const methods?
		constexpr operator       typename Impl::native_type& ()       { return Impl::native(); }

		// The default op= is fine!

		// The default op== too, but that's not gonna be generated automatically... :-/
		bool operator ==  (const Vector_Interface<Impl>&) const = default;
		// OTOH, the meaningless <=> WOULD, so that needs to be deleted:
		auto operator <=> (const Vector_Interface<Impl>&) const = delete;
		// (Ironically, op== WOULD be made automatically available from a <=>, but well...)

		constexpr auto  x() const { return adapter()->_x(); }
		constexpr auto  y() const { return adapter()->_y(); }
		constexpr auto& x()       { return adapter()->_x(); }
		constexpr auto& y()       { return adapter()->_y(); }

		constexpr operator bool () const { return (bool)x() || (bool)y(); }

		constexpr auto  flip() const     { return Vector_Interface(y(), x()); }
		constexpr auto& flip()           { std::swap(x(), y()); return *this; }

		// A common chore (e.g. to avoid subpixel AA-blurring with OpenGL):
		constexpr auto   round() const   { return Vector_Interface(std::round(x()), std::round(y())); }
		constexpr auto&  round()         { x() = std::round(x()); y() = std::round(y()); return *this; }


	protected:
		constexpr auto adapter() const { return static_cast<const Impl*>(this); }
		constexpr auto adapter()       { return static_cast<      Impl*>(this); }

	}; // class Vector_Interface


//----------------------------------------------------------------------------
// "Dictionary" type aliases...
//----------------------------------------------------------------------------

	template <unsigned Dim = 2, Scalar NumT = float> // GPUs prefer float (https://www.reddit.com/r/cpp_questions/comments/1bjl3d5/why_cuda_c_uses_float/)
		requires (Dim == 2) //!! Only 2 dimensions are suppoerted yet
	using Vec  = Vector_Interface<adapter::/*!!math::active_backend::!!*/Vector2_Impl</*!!Dim, !!*/NumT>>; //!! Amend the adapter impl. to also support a Dim param:
	//!!using Vec  = Vector_Interface<adapter::/*math::active_backend::*/Vector_Impl<Dim, NumT>>;

/*!! Can't specialize template aliases! (TODO: Use a type-trait class instead.)
	template <> // GPUs prefer float (https://www.reddit.com/r/cpp_questions/comments/1bjl3d5/why_cuda_c_uses_float/)
	using Vec<3, NumT  = Vector_Interface<adapter::math::active_backend::Vector_Impl<3, NumT>>;
!!*/
	template <Scalar NumT = float>
	using Vec2  = Vector_Interface<adapter::/*math::active_backend::*/Vector2_Impl<NumT>>;

	using fVec2 = Vector_Interface<adapter::/*math::active_backend::*/Vector2_Impl<float>>;
	using iVec2 = Vector_Interface<adapter::/*math::active_backend::*/Vector2_Impl<int>>;
	using uVec2 = Vector_Interface<adapter::/*math::active_backend::*/Vector2_Impl<unsigned>>;
	using dVec2 = Vector_Interface<adapter::/*math::active_backend::*/Vector2_Impl<double>>;

//----------------------------------------------------------------------------
// C++ deduction guides, both for Vec2 v{x, y} to work...
//----------------------------------------------------------------------------

	template <unsigned Dim = 2, Scalar NumT = float>
	Vector_Interface(NumT&& x, NumT&& y) -> Vector_Interface<adapter::/*math::active_backend::*/Vector2_Impl<NumT, Dim>>; //!! Dim is NOT used, just a reminder!

	// The default null vector is float:
	//!! Except... This seems to not do anything with GCC! Vec2{1, 2} is still not 
	template <unsigned Dim = 2, Scalar NumT = float>
	Vector_Interface() -> Vector_Interface<adapter::/*math::active_backend::*/Vector2_Impl<float, Dim>>; //!! Dim is NOT used, just a reminder!

} // SAL//!!::math


//-------------------------------------------------------------------
// Operators (as free functions)...
//-------------------------------------------------------------------

#define _VECTOR_OP  template <Vector V> constexpr auto
//!!OLD:
//!!#define _VECTOR_OP  template <unsigned Dim = 2, Scalar NumT = float> constexpr auto
//!!#define _VECTOR_T   Vec<Dim, NumT>

namespace SAL//!!::math
{
	//!! template <Vector V> using VectorArg = std::conditional_t<SmallVector<V>, V, const V&>;
	
	//!! There should be automatic dispatching on object size (sizeof(v) > sizeof(void*, or e.g. twice that size) 
	//!! for switching to const Vector& instead of by-val!

	_VECTOR_OP operator + (V a, V b) { return V( a.x() + b.x(), a.y() + b.y() ); }
	_VECTOR_OP operator - (V a, V b) { return V( a.x() - b.x(), a.y() - b.y() ); }

	// Dot prod.
	_VECTOR_OP operator * (V a, V b) { //static_assert(false, "WHO'S TRIGGERING THIS ACCIDENTALLY?! :) ");
		return typename V::number_type( a.x() * b.x() + a.y() * b.y() ); }
		//!!?? Braces with V::number_type{...} failed to compile! Why exactly?

	// Binary scalar ops.
	_VECTOR_OP operator + (V v, Scalar auto scalar) { return V( v.x() + scalar, v.y() + scalar ); } // Commutative!
	_VECTOR_OP operator + (Scalar auto scalar, V v) { return V( v.x() + scalar, v.y() + scalar ); }
	_VECTOR_OP operator - (V v, Scalar auto scalar) { return V( v.x() - scalar, v.y() - scalar ); }
	_VECTOR_OP operator / (V v, Scalar auto scalar) { return V( v.x() / scalar, v.y() / scalar ); }
//!!	_VECTOR_OP operator * (V v, Scalar auto scalar) { return V( v.x() * scalar, v.y() * scalar ); } // Commutative!
//!!	_VECTOR_OP operator * (Scalar auto scalar, V v) { return V( v.x() * scalar, v.y() * scalar ); }
	_VECTOR_OP operator * (V v, Scalar auto scalar) { return V( typename V::number_type(v.x() * scalar), typename V::number_type(v.y() * scalar) ); } // Commutative!
	_VECTOR_OP operator * (Scalar auto scalar, V v) { return V( typename V::number_type(v.x() * scalar), typename V::number_type(v.y() * scalar) ); }
/*!!??
	// Supporting mixed number types -- multiplying with a non-NumT scalar would require a
	// tedious explicit conversion (involving dismantling the vector) without these!
	_VECTOR_OP operator * (V v, int scalar)    { return V( typename V::number_type(v.x() * scalar), typename V::number_type(v.y() * scalar) ); } // Commutative!
	_VECTOR_OP operator * (int scalar, V v)    { return V( typename V::number_type(v.x() * scalar), typename V::number_type(v.y() * scalar) ); }
	_VECTOR_OP operator * (V v, float scalar)  { return V( typename V::number_type(v.x() * scalar), typename V::number_type(v.y() * scalar) ); } // Commutative!
	_VECTOR_OP operator * (float scalar, V v)  { return V( typename V::number_type(v.x() * scalar), typename V::number_type(v.y() * scalar) ); }
	_VECTOR_OP operator * (V v, double scalar) { return V( typename V::number_type(v.x() * scalar), typename V::number_type(v.y() * scalar) ); } // Commutative!
	_VECTOR_OP operator * (double scalar, V v) { return V( typename V::number_type(v.x() * scalar), typename V::number_type(v.y() * scalar) ); }
??!!*/
 	// Unary -
 	_VECTOR_OP operator - (V v) { return V( -v.x(), -v.y() ); }

	//
	// OP= operators...
	//
	// Defined separately to keep them optional (!!after moving them to another header!!)
	//
	_VECTOR_OP & operator += (V& self, V v) { self.x() += v.x(); self.y() += v.y(); return self; }
	_VECTOR_OP & operator -= (V& self, V v) { self.x() -= v.x(); self.y() -= v.y(); return self; }
	_VECTOR_OP & operator *= (V& self, V v) { self.x() *= v.x(); self.y() *= v.y(); return self; }
	_VECTOR_OP & operator /= (V& self, V v) { self.x() /= v.x(); self.y() /= v.y(); return self; }
	// Scalar ops.
	_VECTOR_OP & operator += (V& self, Scalar auto scalar) { self.x() += scalar; self.y() += scalar; return self; }
	_VECTOR_OP & operator -= (V& self, Scalar auto scalar) { self.x() -= scalar; self.y() -= scalar; return self; }
	_VECTOR_OP & operator *= (V& self, Scalar auto scalar) { self.x() *= scalar; self.y() *= scalar; return self; }
	_VECTOR_OP & operator /= (V& self, Scalar auto scalar) { self.x() /= scalar; self.y() /= scalar; return self; }

} // namespace SAL//!!::math


//----------------------------------------------------------------------------
// Op << and >>...
//----------------------------------------------------------------------------
#ifdef SAL_VECTOR_STREAMABLE
# include <iostream>
  namespace SAL//!!::math
  {
	_VECTOR_OP & operator << (std::ostream& out, V v) { out <<"("<< v.x() <<", "<< v.y() <<")"; return out; }
//!! Not yet:
//!!	_VECTOR_OP & operator >> (std::istream& in,  V v) { return in; }
  } // namespace SAL//!!::math

#else  // SAL_VECTOR_STREAMABLE

  #include <iosfwd> // ostream, istream are not classes, can't be forward declared! :-/
  namespace SAL//!!::math
  {
	_VECTOR_OP & operator << (std::ostream& out, V) { return out; }
	_VECTOR_OP & operator >> (std::istream& in,  V) { return in;  }
  }
#endif // SAL_VECTOR_STREAMABLE


#undef _VECTOR_OP
//!!OLD: #undef _VECTOR_T


#endif // _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_
