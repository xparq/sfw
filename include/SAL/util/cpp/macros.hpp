﻿#ifndef _OSID87TY287D4Y5J78TY87Y34F07Y78053YTG_
#define _OSID87TY287D4Y5J78TY87Y34F07Y78053YTG_


// Inheritance with no polymorphism; the bases are just mixins:
#define SAL_MIXIN(Base, ...) public Base __VA_OPT__(,) __VA_ARGS__


// Cast `this` to const:
#define SAL_CONST_THIS() /*!! Wow... This is fkn' nuts... !!*/ \
		( const_cast<std::remove_reference<decltype(*this)>::type const*>(this) ) //!!?? Seriously?! :D


#endif // _OSID87TY287D4Y5J78TY87Y34F07Y78053YTG_
