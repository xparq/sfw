#ifndef _GIHDCIUAYSRFTNIWXUYCWVEFTB76Q3XFNI78W6RCTFQ763BRCYT67EN_
#define _GIHDCIUAYSRFTNIWXUYCWVEFTB76Q3XFNI78W6RCTFQ763BRCYT67EN_

//============================================================================
// Default built-in data "plugin" for Vector
//
// To be used as a base class (for automatic support for .x, .y ... access and
// structured binding!), or as data packets for exchange via C++ trivial types.
//============================================================================

namespace VEC_NAMESPACE {

//----------------------------------------------------------------------------
// Generic form -- not specialized for 2D-4D direct coord. names
//----------------------------------------------------------------------------

#include "data-fw.hpp" //! Including only for double-checking: should fail if out of sync!

template <unsigned Dim, Scalar NumT>
struct VectorData {
	constinit const static inline unsigned dim = Dim;
	using number_type = NumT;
	// VectorData is also an adapter to itself!... -> #51:
	using foreign_type = VectorData<Dim, NumT>;
	constexpr       foreign_type& foreign()       { return static_cast<      foreign_type&>(*this); }
	constexpr const foreign_type& foreign() const { return static_cast<const foreign_type&>(*this); }

	NumT n[dim]; //!! No good reason for `std::array<NumT, Dim> n;`

	// "Hopefully" comp.-time accessors...
	template <unsigned i> requires (i < dim) constexpr       NumT& ref()         { return n[i]; }
	template <unsigned i> requires (i < dim) constexpr const NumT& ref() const   { return n[i]; }
	template <unsigned i> requires (i < dim) constexpr       NumT  get() const   { return n[i]; }
	template <unsigned i> requires (i < dim) constexpr auto&       set(NumT val) { n[i] = val; return *this; }

	// Run-time accessors ("hopefully" optimized away, mostly)...
	constexpr       NumT& ref(unsigned i)           { return n[i]; }
	constexpr const NumT& ref(unsigned i) const     { return n[i]; }
	constexpr       NumT  get(unsigned i) const     { return n[i]; }
	constexpr auto&       set(unsigned i, NumT val) { n[i] = val; return *this; }

	// Uniform compile-time accessors
	//!!
	//!! Theoretically, separate runtime and comp-time counterparts would be needed, as it's
	//!! NOT strictly enough to just have them constexpr... It's still pretty stupid: we can't
	//!! request guaranteed comp-time eval from those, OTOH consteval doesn't ALLOW run-time... :-/
	//!! And, also: consteval won't even be possible to use as member functions...
	//!! The proper mechanism would be *specifying* (rather than hoping for...) comp- vs.
	//!! run-time *at the call site*! For the lack of that, let's pretend constexpr is what
	//!! it should ideally be...
	//!! (I've considered making them both templates and plain constexpr functions, so we could
	//!! explicitly decide to use either mode at the call sites, but, of course, using templates
	//!! doesn't change anything: it won't magically turn the same constexpr function to consteval!...)
	//!!
	//!! However... To specialize for direct named coord. access at certain dimensions, we STILL need
	//!! templated versions (overloads) regardless!
	//!!
	//!! Anyway, for completeness, we could add these "guaranteed comp-time accessors", too
	//!! (and have proper diagnostics when used on a non-const object):
	//!!	consteval NumT        cget(unsigned i) const { return n[i]; }
	//!!	consteval void        cset(unsigned i, NumT val) { n[i] = val; }
	//!!	consteval NumT&       cref(unsigned i)       { return n[i]; }
	//!!	consteval const NumT& cref(unsigned i) const { return n[i]; }
	//!!
	/*!! But, then again, we could also just do sg. like this in the specialized templates:
	constexpr NumT get(unsigned i) const {
		if consteval {
			     if (i == 1) return n[0];
			else if (i == 2) return n[1];
			else if (i == 3) return n[2];
			else throw std::out_of_range("Index out of range"); // C++ can't do static_assert specifically for a branch like this...
		} else {
			throw std::logic_error("This function must be called in a compile-time context!");
		}
	}
	!!*/
};


//----------------------------------------------------------------------------
// Specializations for directly accessible coords in 2 - 4 dims...
//----------------------------------------------------------------------------

#ifndef VEC_NO_DIRECT_COORDS
//!!
//!! But direct coord. access for desig. init. is STILL REQUIRED for the coord.-wise ctor in Vector!
//!! So, if disabling this (direct named-coord. support), then that must be disabled, too!
//!!
template <Scalar NumT> struct VectorData<2, NumT> {
	constinit const static inline unsigned dim = 2;
	using number_type = NumT;
	// -> #51:
	using foreign_type = VectorData<dim, NumT>;
	constexpr       foreign_type& foreign()       { return static_cast<      foreign_type&>(*this); }
	constexpr const foreign_type& foreign() const { return static_cast<const foreign_type&>(*this); }

	NumT x, y;

	template <unsigned i> requires (i < dim) constexpr       NumT& ref()         { if constexpr (i == 0) return x; else if constexpr (i == 1) return y; }
	template <unsigned i> requires (i < dim) constexpr const NumT& ref() const   { if constexpr (i == 0) return x; else if constexpr (i == 1) return y; }
	template <unsigned i> requires (i < dim) constexpr       NumT  get() const   { if constexpr (i == 0) return x; else if constexpr (i == 1) return y; }
	template <unsigned i> requires (i < dim) constexpr auto&       set(NumT val) { if constexpr (i == 0) x = val;  else if constexpr (i == 1) y = val; return *this; }

	constexpr       NumT& ref(unsigned i)           { assert(i < dim); return i ? y : x; }
	constexpr const NumT& ref(unsigned i) const     { assert(i < dim); return i ? y : x; }
	constexpr       NumT  get(unsigned i) const     { assert(i < dim); return i ? y : x; }
	constexpr auto&       set(unsigned i, NumT val) { assert(i < dim); if (i) y = val; else x = val; return *this; }
};
static_assert(UniformVectorData<VectorData<2, /*dummy*/float>>); // Not VectorClass, for the lack of a memberwise ctor!

template <Scalar NumT> struct VectorData<3, NumT>  {
	//!! Should inherit & extend VectorData2, ideally, but C++ would then put a spoke in the wheels and kill the aggreg. init syntax!... :-/
	constinit const static inline unsigned dim = 3;
	using number_type = NumT;
	// -> #51:
	using foreign_type = VectorData<dim, NumT>;
	constexpr       foreign_type& foreign()       { return static_cast<      foreign_type&>(*this); }
	constexpr const foreign_type& foreign() const { return static_cast<const foreign_type&>(*this); }

	NumT x, y, z;

	template <unsigned i> requires (i < dim)
	constexpr       NumT& ref()         { if constexpr (i == 0) return x; else if constexpr (i == 1) return y; else if constexpr (i == 2) return z; }

	template <unsigned i> requires (i < dim)
	constexpr const NumT& ref() const   { if constexpr (i == 0) return x; else if constexpr (i == 1) return y; else if constexpr (i == 2) return z; }

	template <unsigned i> requires (i < dim)
	constexpr       NumT  get() const   { if constexpr (i == 0) return x; else if constexpr (i == 1) return y; else if constexpr (i == 2) return z; }

	template <unsigned i> requires (i < dim)
	constexpr auto&       set(NumT val) { if constexpr (i == 0) x = val;  else if constexpr (i == 1) y = val;  else if constexpr (i == 2) z = val; return *this; }

	constexpr       NumT& ref(unsigned i)           { assert(i < dim); return i ? (i==2 ? z : y) : x; }
	constexpr const NumT& ref(unsigned i) const     { assert(i < dim); return i ? (i==2 ? z : y) : x; }
	constexpr       NumT  get(unsigned i) const     { assert(i < dim); return i ? (i==2 ? z : y) : x; }
	constexpr auto&       set(unsigned i, NumT val) { assert(i < dim); if (i==0) x = val; else if (i==1) y = val; else if (i==2) z = val; return *this; }
};
static_assert(UniformVectorData<VectorData<3, /*dummy*/float>>); // Not VectorClass, for the lack of a memberwise ctor!

template <Scalar NumT> struct VectorData<4, NumT> {
	//!! See comment at 3D!...
	constinit const static inline unsigned dim = 4;
	using number_type = NumT;
	// -> #51:
	using foreign_type = VectorData<dim, NumT>;
	constexpr       foreign_type& foreign()       { return static_cast<      foreign_type&>(*this); }
	constexpr const foreign_type& foreign() const { return static_cast<const foreign_type&>(*this); }

	NumT x, y, z, w;

	template <unsigned i> requires (i < dim)
	constexpr NumT& ref() {
		if constexpr (i == 0) return x; else if constexpr (i == 1) return y; else if constexpr (i == 2) return z; else if constexpr (i == 3) return w;
	}
	template <unsigned i> requires (i < dim)
	constexpr const NumT& ref() const {
		if constexpr (i == 0) return x; else if constexpr (i == 1) return y; else if constexpr (i == 2) return z; else if constexpr (i == 3) return w;
	}
	template <unsigned i> requires (i < dim)
	constexpr NumT get() const {
		if constexpr (i == 0) return x; else if constexpr (i == 1) return y; else if constexpr (i == 2) return z; else if constexpr (i == 3) return w;
	}
	template <unsigned i> requires (i < dim)
	constexpr auto& set(NumT val) {
		if constexpr (i == 0) x = val;  else if constexpr (i == 1) y = val;  else if constexpr (i == 2) z = val;  else if constexpr (i == 3) w = val;
		return *this;
	}

	//!!?? Is this any better: { switch(i){ default: return x; case 1: return y; case 2: return z; case 3: return w; } }
	constexpr       NumT& ref(unsigned i)           { assert(i < dim); return i ? (i==3 ? w : (i==2 ? z : y)) : x; }
	constexpr const NumT& ref(unsigned i) const     { assert(i < dim); return i ? (i==3 ? w : (i==2 ? z : y)) : x; }
	constexpr       NumT  get(unsigned i) const     { assert(i < dim); return i ? (i==3 ? w : (i==2 ? z : y)) : x; }
	constexpr auto&       set(unsigned i, NumT val) { assert(i < dim);
		switch(i){ case 0: x = val; break; case 1: y = val; break; case 2: z = val; break; case 3: z = val; } return *this; }
};
static_assert(UniformVectorData<VectorData<4, /*dummy*/float>>); // Not VectorClass, for the lack of a memberwise ctor!

#endif // VEC_NO_DIRECT_COORDS


} // namespace VEC_NAMESPACE


#endif // _GIHDCIUAYSRFTNIWXUYCWVEFTB76Q3XFNI78W6RCTFQ763BRCYT67EN_
