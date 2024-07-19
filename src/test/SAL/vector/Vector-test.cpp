#define SAL_VECTOR_STREAMABLE
#include "SAL/math/Vector.hpp"

#include <iostream>
#include <fstream> // just for updating the run-count file :)
#include <type_traits> // is_same

using namespace std;
using namespace SAL;


//!!Nope, can only be a class member:
// auto operator SFML_to_Vec2(sf::Vector2f sfml_vector) { return Vec2(sfml_vector); }


//----------------------------------------------------------------------------
// Types...

// Deduction from params
static_assert(is_same_v<decltype(Vec2(1, 2)),     Vec2<int>>);
static_assert(is_same_v<decltype(Vec2(1u, 2u)),   Vec2<unsigned>>);
static_assert(is_same_v<decltype(Vec2(1.f, 2.f)), Vec2<float>>);
static_assert(is_same_v<decltype(Vec2(1.0, 2.0)), Vec2<double>>);
static_assert(is_same_v<decltype(Vec2{1, 2}),     Vec2<int>>);
static_assert(is_same_v<decltype(Vec2{1u, 2u}),   Vec2<unsigned>>);
static_assert(is_same_v<decltype(Vec2{1.f, 2.f}), Vec2<float>>);
static_assert(is_same_v<decltype(Vec2{1.0, 2.0}), Vec2<double>>);

// Deduction "despite" params (having different type than the class NumT)
// - explicit templ. type:
static_assert(is_same_v<decltype(Vec2<int>(1.f, 2.f)),  Vec2<int>>);
static_assert(is_same_v<decltype(Vec2<unsigned>(1, 2)), Vec2<unsigned>>);
static_assert(is_same_v<decltype(Vec2<float>(1, 2)),    Vec2<float>>);
static_assert(is_same_v<decltype(Vec2<double>(1u, 2u)), Vec2<double>>);
static_assert(is_same_v<decltype(Vec2<int>{1.f, 2.f}),  Vec2<int>>);
static_assert(is_same_v<decltype(Vec2<unsigned>{1, 2}), Vec2<unsigned>>);
static_assert(is_same_v<decltype(Vec2<float>{1, 2}),    Vec2<float>>);
static_assert(is_same_v<decltype(Vec2<double>{1u, 2u}), Vec2<double>>);
// - typename-driven:
static_assert(is_same_v<decltype(iVec2(1.f, 2.f)), Vec2<int>>);
static_assert(is_same_v<decltype(uVec2(1, 2)),     Vec2<unsigned>>);
static_assert(is_same_v<decltype(fVec2(1, 2)),     Vec2<float>>);
static_assert(is_same_v<decltype(dVec2(1u, 2u)),   Vec2<double>>);
static_assert(is_same_v<decltype(iVec2{1.f, 2.f}), Vec2<int>>);
static_assert(is_same_v<decltype(uVec2{1, 2}),     Vec2<unsigned>>);
static_assert(is_same_v<decltype(fVec2{1, 2}),     Vec2<float>>);
static_assert(is_same_v<decltype(dVec2{1u, 2u}),   Vec2<double>>);


Vec<2> vec_2_float;      static_assert(is_same_v<decltype(vec_2_float), Vec2<float>>);
Vec<2, int> vec_2_int;
Vec2<float> Vec2_float;

//!! These don't compile with MSVC! In fact, it even crashes! :)
#ifndef _MSC_VER
Vec2 Vec2_float_implied; static_assert(is_same_v<decltype(Vec2_float), decltype(Vec2_float_implied)>);
Vec implicit_2d_null_float;
	static_assert(is_same_v<decltype(implicit_2d_null_float), Vec2<float>>);
Vec implicit_2d_double = {111.0, 222.0}; //!! Should warn about the narrowing conversion!
	static_assert(is_same_v<decltype(implicit_2d_double), Vec2<double>>);
/*
	src/test/SAL/vector/Vector-test.cpp(23): error C2641: cannot deduce template arguments for 'SAL::Vec2'
	src/test/SAL/vector/Vector-test.cpp(23): error C3203: 'Vec2': unspecialized alias template can't be us
	ed as a template argument for template parameter '<unnamed-symbol>', expected a real type
	src/test/SAL/vector/Vector-test.cpp(23): error C2607: static assertion failed
	src/test/SAL/vector/Vector-test.cpp(24): error C2641: cannot deduce template arguments for 'SAL::Vec'
	src/test/SAL/vector/Vector-test.cpp(25): error C3203: 'Vec': unspecialized alias template can't be use
	d as a template argument for template parameter '<unnamed-symbol>', expected a real type
	src/test/SAL/vector/Vector-test.cpp(25): error C2607: static assertion failed
	src/test/SAL/vector/Vector-test.cpp(26): fatal error C1907: unable to recover from previous error(s);
	stopping compilation
	INTERNAL COMPILER ERROR in 'C:\SW\devel\tool\lang\MSVC+WinSDK\current\VC\Tools\MSVC\14.35.32215\bin\Ho
	stX64\x64\cl.exe'
*/
#endif // _MSC_VER

//!! Neither does this:
#ifndef _MSC_VER
Vec implicit_2d_float = {111.0f, 222.0f}; //!! Should warn about the narrowing conversion!
	static_assert(is_same_v<decltype(implicit_2d_float), Vec2<float>>);
/*
	src/test/SAL/vector/Vector-test.cpp(48): fatal error C1001: Internal compiler error.
	(compiler file 'msc1.cpp', line 1584)
	 To work around this problem, try simplifying or changing the program near the locations listed above.
	If possible please provide a repro here: https://developercommunity.visualstudio.com
	Please choose the Technical Support command on the Visual C++
	 Help menu, or open the Technical Support help file for more information
	INTERNAL COMPILER ERROR in 'C:\SW\devel\tool\lang\MSVC+WinSDK\current\VC\Tools\MSVC\14.35.32215\bin\Ho
	stX64\x64\cl.exe'
*/
#endif // _MSC_VER

//!! Not implemented yet:
//!!Vec<3> floatvect3;

Vec2 v_braces_ctor_deduct{1, 2};  static_assert(is_same_v<decltype(v_braces_ctor_deduct), Vec2<int>>);
Vec2 v_parens_ctor_deduct(3, 4);  static_assert(is_same_v<decltype(v_parens_ctor_deduct), Vec2<int>>);

Vec2<int> null_int2_b{};  static_assert(is_same_v<decltype(null_int2_b), Vec2<int>>);
Vec2<int> null_int2_p;    static_assert(is_same_v<decltype(null_int2_p), Vec2<int>>);
Vec2 null_int_b{};        static_assert(is_same_v<decltype(null_int_b), Vec2<float>>);
//!! Fails with both MSVC & GCC: they take it as a function decl.:
//!!Vec2 null_int_p();        static_assert(is_same_v<decltype(null_int_p), Vec2<float>>);
#ifndef _MSC_VER
Vec2 null_int_none;       static_assert(is_same_v<decltype(null_int_none), Vec2<float>>);
#endif // _MSC_VER
auto auto_null_int2_b = Vec2<int>{};  static_assert(is_same_v<decltype(auto_null_int2_b), Vec2<int>>);
auto auto_null_int2_p = Vec2<int>();  static_assert(is_same_v<decltype(auto_null_int2_p), Vec2<int>>);
auto auto_null_int_b = Vec2{};        static_assert(is_same_v<decltype(auto_null_int_b), Vec2<float>>);
auto auto_null_int_p = Vec2();        static_assert(is_same_v<decltype(auto_null_int_p), Vec2<float>>);

static_assert(is_same_v<iVec2, Vec2<int>>);
static_assert(is_same_v<fVec2, Vec2<float>>);


//----------------------------------------------------------------------------
// Values...

int main()
{
	//!! For now, SFML is the only backend, so its vector type is just named here directly:
	sf::Vector2f sfv{5.f, 7.f}; static_assert(is_same_v< decltype(sfv), SAL::adapter::/*!!math::active_backend::!!*/Vector2_Impl<float>::native_type >);


	cout << "default: " << Vec2<int>().x() <<", "<< Vec2<int>().y() << '\n';
	cout << "Can also << with SFW_VECTOR_HAS_STREAM_OPS: "
	     << Vec2<int>() <<", "<< Vec2<float>(1.23, 45.34) << '\n';
	// SFML doesn't have <<:
	//cout << sfv;

	Vec2 vint{4, 5};         static_assert(is_same_v<decltype(vint),     Vec2<int>>);
	Vec2 vdbl{4.0, 5.6};     static_assert(is_same_v<decltype(vdbl),     Vec2<double>>);
	Vec2 vdbl_alt(4.0, 5.6); static_assert(is_same_v<decltype(vdbl_alt), Vec2<double>>);
	Vec2 vflt{4.0f, 5.6f};
	// SFML doesn't support this deduction, BTW:
	//sf::Vector2 sfv{5, 7};
	//sf::Vector2 sfv{5.f, 7f};
	fVec2 vf_from_dbl{4.0, 5.6};
	// Should fail:
	//Vec2 vx{4.0, 5.6f};

	cout << "int vector (size: "<< sizeof(vint) <<"): " << vint << '\n';
	cout << "float vector (size: "<< sizeof(vflt) <<"): " << vflt << '\n';
	cout << "double vector (size: "<< sizeof(vdbl) <<"): " << vdbl << '\n';
	cout << "float-from-double vector (size: "<< sizeof(vf_from_dbl) <<"): " << vf_from_dbl << '\n';


	//--------------------------------------------------------------------
	// Copy & assignment:
	auto vcopy{Vec2(1,2)};
	cout << "Copied a temp: " << vcopy << '\n';
	auto vcopy2{vcopy};
	cout << "Copied an lval: " << vcopy2 << '\n';

	// Copy from the backend (SFML) type:
	cout << "Copied from SFML: " << fVec2{sfv} << '\n';

	//!! This indirect deduction of numtype doesn't work in C++ (yet?):
	//!!Vec2{sfv};

	// Convert to backend types:
	iVec2 ivec_to_cvt{1, 2};
		sf::Vector2i sfvi_receiving_1 = ivec_to_cvt;       static_assert(is_same_v<decltype(sfvi_receiving_1), sf::Vector2i>);
		sf::Vector2i sfvi_receiving_2(ivec_to_cvt);        static_assert(is_same_v<decltype(sfvi_receiving_2), sf::Vector2i>);
		auto sfvi_receiving_3 = sf::Vector2i(ivec_to_cvt); static_assert(is_same_v<decltype(sfvi_receiving_3), sf::Vector2i>);
		auto sfvi_receiving_4 = (sf::Vector2i)ivec_to_cvt; static_assert(is_same_v<decltype(sfvi_receiving_4), sf::Vector2i>);


	//--------------------------------------------------------------------
	// Assignment...
	fVec2 ass1; ass1 = fVec2{1, 2};
	cout << "Assigned (should be 1,2): " << ass1 << '\n';

	//--------------------------------------------------------------------
	// Manipulating the native type:
	auto v_23 = Vec2{2, 3};
	v_23.native().y = 4;
	cout << "(2, 3) changed (y := 4) via native() access: " << v_23 << '\n';


	//--------------------------------------------------------------------
	// Convert between different number types:
	iVec2 ivec2f{1, 2};
		// Implicit conv. - no longer enabled:
		//fVec2 fvec_receiving_1 = ivec2f;             static_assert(is_same_v<decltype(fvec_receiving_1), fVec2>);
		fVec2 fvec_receiving_2(ivec2f);              static_assert(is_same_v<decltype(fvec_receiving_2), fVec2>);
		auto fvec_receiving_3 = fVec2(ivec2f);       static_assert(is_same_v<decltype(fvec_receiving_3), fVec2>);
		auto fvec_receiving_4 = (fVec2)ivec2f;       static_assert(is_same_v<decltype(fvec_receiving_4), fVec2>);
	iVec2 ivfromdv = iVec2(100.9, -200.9);
		assert(ivfromdv.x() ==  100);
		assert(ivfromdv.y() == -200);

	//- - - - - - - - - - - - - -
	// Type conversion - errors:
	//- - - - - - - - - - - - - -

	// Implicit conv. is not enabled (will need another Vector type for that! -> #443):
	//fVec2 fvec_receiving_1 = ivec2f;

	//!! Not checked currently! There should be explicitly selectable checking versions!
	auto ivfromdv_overflow = iVec2(1.0e100, -2.0e200);
	cout << "*SILENT* overflowing conv. of " << dVec2(1.0e100, -2.0e200) << " to " << ivfromdv_overflow << "! :-( \n";


	//--------------------------------------------------------------------
	// Op ==
	cout << "(1, 2) == (1, 2) ? " << (iVec2(1, 2) == iVec2(1, 2)) << '\n';
	cout << "(1, 2) == (2, 3) ? " << (iVec2(1, 2) == iVec2(2, 3)) << '\n';

	//--------------------------------------------------------------------
	// Math operators...
	//!! ...

	//--------------------------------------------------------------------
	// Misc. operators...
	auto flipped = fVec2(6, 7).flip();
	cout << "(6, 7) flipped:" << flipped << '\n';


	// No .0 formatting, despite not being int! :-o
	//double x = 1; cout << x << '\n'; cout << sizeof(x) << '\n';

ofstream("run_count.tmp", std::ios_base::app) << "x";
}
