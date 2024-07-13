#ifndef _DFGNUDOSTDF8DM2HE87TYG87HY34870DFHYE98_
#define _DFGNUDOSTDF8DM2HE87TYG87HY34870DFHYE98_

//============================================================================
#ifdef DEBUG
//============================================================================

//
// Just have stderr handy...
//
//!! Grow a proper logging facility instead (or in addition), of course!
//!! (Don't forget to use OutputDebugString on Windows, like in my old DBG header!)
//
#include <iostream>
	using std::cerr, std::endl, std::hex;

//
// Pimped asserts from Chris Wellons (Skeeto) -- https://nullprogram.com/blog/2022/06/26/
// Instead of #include <cassert>, this breaks (for debugging) instead of printing and terminating.
//

//!!?? WTF, I sill got the stock assert despite this?!
//!!??#  undef assert // SFML could've already included <cassert> before we get here...
//!!??

#  if __GNUC__
#    define SAL_ASSERT(c) if (!(c)) __builtin_trap()
#  elif _MSC_VER
#    define SAL_ASSERT(c) if (!(c)) __debugbreak()
#  else
#    define SAL_ASSERT(c) if (!(c)) *(volatile int *)0 = 0
#  endif

//============================================================================
#else // DEBUG
//============================================================================


#  define SAL_ASSERT(c)


//============================================================================
#endif // DEBUG
//============================================================================

#endif // _DFGNUDOSTDF8DM2HE87TYG87HY34870DFHYE98_
