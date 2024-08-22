#ifndef _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_
#define _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_

#include "../vec/adapter/sfml.hpp"

namespace SAL/*!!::math!!*/::adapter
{
  namespace SFML
  {
	template <unsigned Dim, VEC_NAMESPACE::Scalar NumT>
	using VectorAdapter = VEC_NAMESPACE::adapter::SFML<Dim, NumT>;
  } // namespace SFML

	// Save it as the "default", to a backend-agnostic symbol:
	template <unsigned Dim, VEC_NAMESPACE::Scalar NumT>
	using VectorAdapter = SFML::VectorAdapter<Dim, NumT>;

} // namespace SAL/*!!::math!!*/::adapter


#endif // _RVSRTYNFDGYSVEC336AB64N7W568VW47W57RT7_
