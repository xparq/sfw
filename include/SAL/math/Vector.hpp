//----------------------------------------------------------------------------
// Platform-independent vector type (the interface half of the adapter
// -- see the ..._Impl class for the implementation half, #included below)
//
// Define SAL_VECTOR_STREAMABLE to add << and >> (otherwise they are no-ops).
//
//----------------------------------------------------------------------------

#ifndef _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_
#define _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_

#define VEC_NO_DIRECT_COORDS // Acccess only via .x() (not .x)!
#include "Vector/vec/adapter.hpp"

#ifdef SAL_VECTOR_STREAMABLE
# include "Vector/vec/io.hpp"
#else
//# define VEC_NO_IOS // No need if io.hpp isn't even included.
#endif


//----------------------------------------------------------------------------
// The backend-specific adapter impl.:
#include "SAL.hpp"
#include SAL_ADAPTER(math/Vector)


namespace SAL//!!::math
{
	using VEC_NAMESPACE::Scalar;

//----------------------------------------------------------------------------
// "Dictionary" type aliases...
//----------------------------------------------------------------------------
	template <unsigned Dim = 2, Scalar NumT = float>
	using Vec = VEC_NAMESPACE::Vector<Dim, NumT, adapter::VectorAdapter>;

	template <Scalar NumT = float>
	using Vec2  = Vec<2, NumT>;

	using fVec2 = Vec2<float>;
	using iVec2 = Vec2<int>;
	using uVec2 = Vec2<unsigned>;
	using dVec2 = Vec2<double>;

} // SAL//!!::math


#endif // _CMOIWD756B87NC2X45TORIUTLBYGH67845NET_
