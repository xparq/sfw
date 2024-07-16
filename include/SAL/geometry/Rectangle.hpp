﻿//----------------------------------------------------------------------------
// Platform-independent rectangle type wrapper (the interface half of the adapter
// -- see the ..._Impl class for the implementation half, #included below)
//----------------------------------------------------------------------------

#ifndef _RV56988BFU68MOUJH98V73903906IDFGDFG45Y_
#define _RV56988BFU68MOUJH98V73903906IDFGDFG45Y_


//!!--------------------------------------------------------------------------
//!!?? Move this to another header? (Used by both the adapter interface and impl.)
//!!   Possibly along with Vector and Scalar then!
//!!
//!!   And reconcile the different namespaces then!!!
//!!
#include "SAL/math/Vector.hpp"

namespace SAL::geometry
{
	template <class R>
	concept Rectangle = requires(R r) {
//		typename R::number_type;
//		typename R::vector_type;
		r.x(); r.y(); r.dx(); r.dy();
	};

} // namespace SAL::geometry
//!!--------------------------------------------------------------------------


// The backend-specific adapter impl.:
#include "SAL.hpp"
#include SAL_ADAPTER(geometry, Rectangle_Impl.hpp)

#include <utility> // move
#include <type_traits> // is_same


namespace SAL::geometry
{
	template <class Impl>
	class Rectangle_Interface : public Impl
 	{
	public:
		constexpr Rectangle_Interface() = default;

		//!!?? Accept native vector types (our Vector auto-converts to those, so they work, too),
		//!!?? but that might not actually be a good thing -- could be confusingly permissive, and
		//!!?? also untenable with more complex classes, which then would also be expected to support
		//!!?? all sorts of other native types transparently (with minor combinatoric explosions)!
		//!!?? (And the confusion part: generic APIs are not expected to work with "random" foreign
		//!!?? types; however, supporting the types of the current backend could be excused.)
		constexpr Rectangle_Interface(Impl::vector_type position, Impl::vector_type size)
			: Impl(position.native(), size.native()) {}

		constexpr Rectangle_Interface(Impl::vector_type size)
			: Impl({0,0}, size.native()) {}

/*!! OLD:
		// Convert from lval with different number type:
		template <class OtherRectT>// requires (!std::is_same_v<typename OtherRectT::number_type, typename Impl::number_type>)
		constexpr Rectangle_Interface(const OtherRectT& other) //! `const` would break rval inputs like auto r = fRect(), despite having an rval ctor, too! :-o
			: Impl( {(typename Impl::number_type)other.x(),     (typename Impl::number_type)other.y()},
			        {(typename Impl::number_type)other.width(), (typename Impl::number_type)other.height()} ) { static_assert(false, "HERE"); }
!!*/
		// Convert from rval with different number type:
			//!! These versions both failed to enable this ctor for e.g. const iRect const_ir; auto cfr = fRect(const_ir4);
			//	template <Rectangle OtherRectT> Rectangle_Interface(OtherRectT&& other) ...
			//	... requires (!std::is_same_v<typename OtherRectT::number_type, typename Impl::number_type>)
		template <Rectangle OtherRectT,
			typename = std::enable_if_t< !std::is_same_v<typename std::decay_t<OtherRectT>::number_type, typename Impl::number_type> >
				//!! decay_t (or at least std::remove_cvref_t) is CRITICAL for accepting both const/non-const OtherRect ctor args.! :-o
		>
		constexpr Rectangle_Interface(OtherRectT&& other) //!!{ static_assert(false, "HERE"); }
			: Impl( {(typename Impl::number_type)std::forward<OtherRectT>(other).x(),     (typename Impl::number_type)std::forward<OtherRectT>(other).y()},
			        {(typename Impl::number_type)std::forward<OtherRectT>(other).width(), (typename Impl::number_type)std::forward<OtherRectT>(other).height()} ) {} //!!{ static_assert(false, "HERE"); }

		//!! Also: if the converting ctor above indeed transparently covers rvals/lvals & const/non-consts,
		//!! then do the same for the plain (Other == Self) copy/move ctors below:

//!!?? WHY ARE THESE NEVER ACTUALLY SELECTED IN GCC, BUT ARE IN MSVC?! :-o (Not a failed assert ever! :-o )
		template <Rectangle SameRectT,
			typename = std::enable_if_t< std::is_same_v<typename std::decay_t<SameRectT>::number_type, typename Impl::number_type> >
				//!! decay_t (or at least std::remove_cvref_t) is CRITICAL for accepting both const/non-const OtherRect ctor args.! :-o
		>
		constexpr Rectangle_Interface(const SameRectT& other) : Impl(std::forward(other.native())) { static_assert(false, "HERE"); }
/*!! OLD:
		// Copy:
		template <class SameRectT> requires (std::is_same_v<typename SameRectT::number_type, typename Impl::number_type>)
		constexpr Rectangle_Interface(const SameRectT& other) : Impl(other.native()) { static_assert(false, "HERE"); }

		// Move (still should just be a copy though):
		template <class SameRectT> requires (std::is_same_v<typename SameRectT::number_type, typename Impl::number_type>)
		constexpr Rectangle_Interface(SameRectT&& other) : Impl(std::move(other.native())) { static_assert(false, "HERE"); }
!!*/
		// All other signatures are passed to the native impl.:
		using Impl::Impl; // Elevate the backend-specific ctors for implicit backend -> abstract conversions!


		// Convert to the native type:
		constexpr operator       typename Impl::native_type& ()       { return Impl::native(); }
		constexpr operator const typename Impl::native_type& () const { return Impl::native(); }


		// The default op= is fine!

		//!!
		//!! Some ops. would be suboptimal for native rects of {x1,y1, x2,y2} with this simplistic API!
		//!! Worse yet: some are not even implemented (or implementable (sanely)) for that layout! :-o
		//!!

		constexpr auto  x()  const { return adapter()->_x(); }
		constexpr auto  y()  const { return adapter()->_y(); }
		constexpr auto  dx() const { return adapter()->_dx(); }
		constexpr auto  dy() const { return adapter()->_dy(); }

		constexpr auto& x()        { return adapter()->_x(); }
		constexpr auto& y()        { return adapter()->_y(); }
		constexpr auto& dx()       { return adapter()->_dx(); }
		constexpr auto& dy()       { return adapter()->_dy(); }

		// Alternative accessors & setters...

		constexpr auto  width()  const { return dx(); }
		constexpr auto  height() const { return dy(); }
		constexpr auto  left()   const { return x(); }
		constexpr auto  top()    const { return y(); }
		constexpr auto  right()  const { return x() + dx(); }
		constexpr auto  bottom() const { return y() + dy(); }

		constexpr auto& width()        { return dx(); }
		constexpr auto& height()       { return dy(); }
		constexpr auto& left()         { return x(); }
		constexpr auto& top()          { return y(); }
		constexpr auto& right()        { static_assert(false, "Not implemented!"); }
		constexpr auto& bottom()       { static_assert(false, "Not implemented!"); }

		constexpr auto& width(Scalar auto n)  { dx() = n; return *this; }
		constexpr auto& height(Scalar auto n) { dy() = n; return *this; }
		constexpr auto& left(Scalar auto n)   { x() = n; return *this; }
		constexpr auto& top(Scalar auto n)    { y() = n; return *this; }
		constexpr auto& right(Scalar auto n)  { dx() = n - x(); return *this; }
		constexpr auto& bottom(Scalar auto n) { dy() = n - y(); return *this; }

		//! Alas, these ones make copies. Use the native() object to access the raw vectors
		//! directly, if you know the configured backend has them.
		constexpr Vec2<typename Impl::number_type> position() const { return {x(), y()}; }
		constexpr Vec2<typename Impl::number_type> size()     const { return {dx(), dy()}; }

		constexpr operator bool () { return (bool)dx() && (bool)dy(); } // Area != 0?
			// So, the position doesn't matter. Also, negative size is OK. Also, not ||, unlike Vector's.

	protected:
		constexpr auto adapter()       { return static_cast<      Impl*>(this); }
		constexpr auto adapter() const { return static_cast<const Impl*>(this); }

	}; // class Rectangle_Interface

//!! Ugh, this is pretty horribly vague here:
//!!	template <Scalar NumT = float>
	constexpr bool operator == (const auto& a, const auto& b) { return a.native() == b.native(); }


//----------------------------------------------------------------------------
//!!?? FIX...
// C++ deduction guides for Rect() and Rect{{x, y}, {dx, dy}} to work...
//----------------------------------------------------------------------------

//!! Using ctors now instead, so DELETE:
//	template <Scalar NumT = float>
//	Rectangle_Interface(Vec2<NumT>&& pos, Vec2<NumT>&& size) -> Rectangle_Interface<adapter::/*active_backend::*/Rectangle_Impl<NumT>>;
//
//	template <Scalar NumT = float>
//	Rectangle_Interface() -> Rectangle_Interface<adapter::/*active_backend::*/Rectangle_Impl<NumT>>;

//!!?? DELETE:
//!!	template <Scalar NumT>
//!!	Rectangle_Interface(const Vec2<NumT>&, const Vec2<NumT>&) -> Rectangle_Interface<adapter::/*active_backend::*/Rectangle_Impl<NumT>>;

//!!?? Is this a viable alternative?
//!!	template <Scalar NumT>
//!!	Rectangle_Interface(Vector auto&& pos, Vector auto&& size) -> Rectangle_Interface<adapter::/*active_backend::*/Rectangle_Impl<NumT>>;

#if 0	//!!?? These didn't help with declaring the cvt ctor with (const OtherT&):
	template <Scalar NumT = float, class OtherRectT>
		requires (!std::is_same_v<typename OtherRectT::number_type, NumT>)
	Rectangle_Interface(OtherRectT&& other) -> Rectangle_Interface<adapter::/*active_backend::*/Rectangle_Impl<NumT>>;

	template <Scalar NumT = float, class OtherRectT>
		requires (!std::is_same_v<typename OtherRectT::number_type, NumT>)
	Rectangle_Interface(const OtherRectT& other) -> Rectangle_Interface<adapter::/*active_backend::*/Rectangle_Impl<NumT>>;

	template <class SameRectT>
	Rectangle_Interface(const SameRectT& other) -> Rectangle_Interface<adapter::/*active_backend::*/Rectangle_Impl<typename SameRectT::number_type>>;
#endif


//----------------------------------------------------------------------------
// Convenience type aliases...
//----------------------------------------------------------------------------

	template <typename NumT = float>
	using  Rect = Rectangle_Interface<adapter::/*geometry::active_backend::*/Rectangle_Impl<NumT>>;

	using fRect = Rectangle_Interface<adapter::/*geometry::active_backend::*/Rectangle_Impl<float>>;
	using iRect = Rectangle_Interface<adapter::/*geometry::active_backend::*/Rectangle_Impl<int>>;
	using uRect = Rectangle_Interface<adapter::/*geometry::active_backend::*/Rectangle_Impl<unsigned>>;
	using dRect = Rectangle_Interface<adapter::/*geometry::active_backend::*/Rectangle_Impl<double>>;


//-------------------------------------------------------------------
// Operators (as free functions)...
//-------------------------------------------------------------------

//!!
//!!?? operator + for union (e.g. for combining AABBs)...
//!!


} // namespace SAL::geometry


#endif // _RV56988BFU68MOUJH98V73903906IDFGDFG45Y_