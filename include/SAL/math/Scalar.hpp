#ifndef _DSSKJHDSJKGHSDJKLGHDSLJKGH8943HT2745H20724H50824HT084_
#define _DSSKJHDSJKGHSDJKLGHDSLJKGH8943HT2745H20724H50824HT084_


#include <type_traits>
#include <concepts>


namespace SAL//!!::math
{

//----------------------------------------------------------------------------
// Alas, no std::number... (https://stackoverflow.com/a/58067611/1479945)
template<typename T> concept Scalar = std::integral<T> || std::floating_point<T>;

} // namespace SAL//!!::math


#endif // _DSSKJHDSJKGHSDJKLGHDSLJKGH8943HT2745H20724H50824HT084_
