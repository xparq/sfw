//----------------------------------------------------------------------------
// Dispatching backend-specific headers (for #include-ing by client code)
//
// Usage examples:
//    #include SFW_ADAPTER_IMPL(math/Vector)           -> sfw/adapter/math/Vector/SFW_CFG_GFX_BACKEND/Impl.hpp
//    #include SFW_ADAPTER_IMPL(math/Vector, Impl.cpp) -> sfw/adapter/math/Vector/SFW_CFG_GFX_BACKEND/Impl.cpp
//    #include SFW_ADAPTER_IMPL(math, Vector.hpp)      -> sfw/adapter/math/SFW_CFG_GFX_BACKEND/Vector.hpp
//    #include SFW_ADAPTER_IMPL(math, Vector.cpp)      -> sfw/adapter/math/SFW_CFG_GFX_BACKEND/Vector.cpp
//
// https://stackoverflow.com/a/50728314/1479945 <- Reminder that `#include MACRO` is a thing...
//----------------------------------------------------------------------------

#ifndef _CPO8E9PC8MER8958YS9VDTNYC980MYX89Q809357W89ENRBY_
#define _CPO8E9PC8MER8958YS9VDTNYC980MYX89Q809357W89ENRBY_


//#define SFW_ADAPTER_IMPL(Component, ...)  SFW_ADAPTER_IMPL1(Component)
//!!#define SFW_ADAPTER_IMPL(Component, Suffix)  _sfw_STR(_sfw_CONCAT(_sfw_COMPONENT_PATH_PREFIX/Component/SFW_CFG_GFX_BACKEND, Suffix))
//!! Also generalize the implicit switcher macro (SFW_CFG_GFX_BACKEND)!
//!!?? Rename to sg. like SFW_BACKEND_PATH(...), or even back to SFW_BACKEND_SPECIFIC? (Or not even BACKEND...)

#define _sfw_EXPAND(X) X
#define _sfw_GET_MACRO(_1, _2, NAME, ...) NAME
#define SFW_ADAPTER_IMPL(...) _sfw_EXPAND(_sfw_GET_MACRO(__VA_ARGS__, SFW_ADAPTER_IMPL_2, SFW_ADAPTER_IMPL_1)(__VA_ARGS__))
//https://stackoverflow.com/a/69945225/1479945

//!! Get rid of this awkward, hardcoded legacy safeguard:
#ifndef SFW_CFG_GFX_BACKEND
//!!Should only be a warning about including some default!
//!!But #warning is c++23(!...), and isn't supported by MSVC yet. :-/
//!!(Note that GCC has supported it for ages.)
//!!#warning No backend has been selected. Use: #include "sfw/cfg/USE_{backendname}"
//!!#warning Selecting SFML as default graphics backend...
#include "sfw/cfg/BACKEND/DEFAULT.h"
#endif


//!! Get these from a common language shim header of sz.lib instead!
//!! The #ifdef is a reminder to support possibly including multiple versions of sz.lib
//!! within the same project, without any self-reflective version-checking mechanism (in sz).
#ifndef  _sz_STR
# define _sz_STR(X) _sz_STR_(X)
# define _sz_STR_(X) #X
# define _sz_CONCAT(X, Y) _sz_CONCAT_(X, Y)
# define _sz_CONCAT_(X, Y) X##Y
#endif //_sz_STR


//----------------------------------------------------------------------------
// Internal helpers

// "DRY" shorthand:
#define SFW_COMPONENT_PATH_PREFIX  sfw/adapter //!! Should probably be a coming macro!

// Create path string from two non-empty, unquoted components:
#define _sfw_PATH_STR(Pre, Post)  _sz_STR(Pre/Post) //!!?? Does it also need to be CONCAT'ed? Seems to work, but: always?! (I.e. spaces, commas in the arguments?!)

#define SFW_ADAPTER_IMPL_1(Component)            _sfw_PATH_STR(SFW_COMPONENT_PATH_PREFIX/Component/SFW_CFG_GFX_BACKEND, Impl.hpp)
#define SFW_ADAPTER_IMPL_2(Component, Filename)  _sfw_PATH_STR(SFW_COMPONENT_PATH_PREFIX/Component/SFW_CFG_GFX_BACKEND, Filename)


#endif // _CPO8E9PC8MER8958YS9VDTNYC980MYX89Q809357W89ENRBY_
