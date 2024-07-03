//----------------------------------------------------------------------------
// Platform-independent vector type wrapper (the interface half of the adapter
// -- see the ..._Impl class for the implementation half, #included below)
//
// Define SFW_VECTOR_STREAMABLE to add << and >> (otherwise they are no-ops).
//
//----------------------------------------------------------------------------

#ifndef _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_
#define _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_


//!!--------------------------------------------------------------------------
//!!?? Move this to another header? (Used by both the adapter interface and impl.)
//!!   Possibly along with the definition of Scalar then (removing it from the
//!!   generic "util" stuff), making it part of this new set of concepts!
//!!
#include "sfw/util/cpp/types.hpp" // Scalar

#include <cmath> // round


namespace sfw//!!::math
{
	template<class V>
	concept Vector = requires(V v) {
		V::dimensions;
		typename V::number_type;
		v.x(); v.y();
	};

	template <class V>
	concept SmallVector = (sizeof(V) <= 2 * sizeof(void*)); // For copy/by-ref optim.

} // namespace sfw::geometry
//!!--------------------------------------------------------------------------


// The backend-specific adapter impl.:
#include "sfw/adapter/dispatch.hpp"
#include SFW_ADAPTER_IMPL(math/Vector)


namespace sfw//!!::math
{
	template <class Impl>
	class Vector_Interface : public Impl
	{
	public:
		using Impl::Impl; // Elevate the backend-specific ctors for implicit backend -> abstract conversions!

		// The default op= is fine!

		// The default op== too, but that's not gonna be generated automatically... :-/
		bool operator ==  (const Vector_Interface<Impl>&) const = default;
		// OTOH, the meaningless <=> WOULD, so that needs to be deleted:
		auto operator <=> (const Vector_Interface<Impl>&) const = delete;
		// (Ironically,, op== WOULD be made automatically available from a <=>, but well...)

		constexpr auto adapter()       { return static_cast<      Impl*>(this); }
		constexpr auto adapter() const { return static_cast<const Impl*>(this); }

		constexpr auto  x() const { return adapter()->_x(); }
		constexpr auto  y() const { return adapter()->_y(); }
		constexpr auto& x()       { return adapter()->_x(); }
		constexpr auto& y()       { return adapter()->_y(); }

		constexpr operator       typename Impl::native_type& ()       { return Impl::native(); }
		constexpr operator const typename Impl::native_type& () const { return Impl::native(); }

		constexpr operator bool () { return (bool)x() || (bool)y(); }

		// A common chore (e.g. to avoid subpixel AA-blurring with OpenGL):
		constexpr auto  round() const { return Vector_Interface(std::round(x()), std::round(y())); }
		constexpr auto& round()       { std::round(x()); std::round(y()); return *this; }

	}; // class Vector_Interface


//----------------------------------------------------------------------------
// Convenience type aliases...
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
} // sfw//!!::math


//-------------------------------------------------------------------
// Operators (as free functions)...
//-------------------------------------------------------------------

#define _VECTOR_OP  template <Vector V> constexpr auto
//!!OLD:
//!!#define _VECTOR_OP  template <unsigned Dim = 2, Scalar NumT = float> constexpr auto
//!!#define _VECTOR_T   Vec<Dim, NumT>

namespace sfw//!!::math
{
	template <Vector V> using VectorArg = std::conditional_t<SmallVector<V>, V, const V&>;
	
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
	_VECTOR_OP operator * (V v, Scalar auto scalar) { return V( v.x() * scalar, v.y() * scalar ); } // Commutative!
	_VECTOR_OP operator * (Scalar auto scalar, V v) { return V( v.x() * scalar, v.y() * scalar ); }
	_VECTOR_OP operator / (V v, Scalar auto scalar) { return V( v.x() / scalar, v.y() / scalar ); }

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


//----------------------------------------------------------------------------
// C++ deduction guide for Vec2 v{x, y} to work...
//----------------------------------------------------------------------------

	template <unsigned Dim = 2, Scalar NumT>
	Vector_Interface(NumT&& x, NumT&& y) -> Vector_Interface<adapter::/*math::active_backend::*/Vector2_Impl<NumT, Dim>>; //!! Dim is NOT unused, just a reminder!
} // namespace sfw//!!::math


//----------------------------------------------------------------------------
// Op << and >>...
//----------------------------------------------------------------------------
#ifdef SFW_VECTOR_STREAMABLE
# include <iostream>
  namespace sfw//!!::math
  {
	_VECTOR_OP & operator << (std::ostream& out, V v) { out <<"("<< v.x() <<", "<< v.y() <<")"; return out; }
	_VECTOR_OP & operator >> (std::istream& in,  V v) {
		static_assert(false, "Vector op<< is not implemented yet!"); //!! It's far less trivial than writing. :-/
		return in;
	}
  } // namespace sfw//!!::math

#else  // SFW_VECTOR_STREAMABLE

  #include <iosfwd> // ostream, istream are not classes, can't be forward declared! :-/
  namespace sfw//!!::math
  {
	_VECTOR_OP & operator << (std::ostream& out, V) { return out; }
	_VECTOR_OP & operator >> (std::istream& in,  V) { return in;  }
  }
#endif // SFW_VECTOR_STREAMABLE


#undef _VECTOR_OP
//!!OLD: #undef _VECTOR_T


#endif // _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_
