#include "sfw/geometry/Rectangle.hpp"

#include <iostream>
#include <fstream> // just for updating the run-count file :)
#include <type_traits> // is_same

using namespace std;
using namespace sfw;
//namespace geo = sfw::geometry;
using namespace sfw::geometry;


//----------------------------------------------------------------------------
auto r_default_p = Rect();       static_assert(is_same_v<decltype(r_default_p),  Rect<float>>);
auto r_default_b = Rect{};       static_assert(is_same_v<decltype(r_default_b),  Rect<float>>);

auto ir_default_p = iRect();     static_assert(is_same_v<decltype(ir_default_p), Rect<int>>);
auto ir_default_b = iRect{};     static_assert(is_same_v<decltype(ir_default_b), Rect<int>>);

auto r0_p  = Rect<int>();
auto r0_b  = Rect<int>{};
Rect<int> tir0_b = {};
iRect     ir0_b  = {};

auto r0_p2 = Rect<int>({}, {});   static_assert(is_same_v<decltype(r0_p2),    Rect<int>>);
auto r0_b2 = Rect<int>{{}, {}};   static_assert(is_same_v<decltype(r0_b2),    Rect<int>>);
iRect NullRect1 = {};             static_assert(is_same_v<decltype(NullRect1), Rect<int>>);
iRect NullRect2 = {{}, {}};       static_assert(is_same_v<decltype(NullRect2), Rect<int>>);
iRect NullRect3 = {{0,0}, {0,0}}; static_assert(is_same_v<decltype(NullRect3), Rect<int>>);


//----------------------------------------------------------------------------
int main()
{
//!! NOT YET (ALSO KINDA MORE AMBIGUOUS THAN {V, V}!):
//!!	iRect ir4 = {1, 2, 3, 4};

	// Default & cvt ctor invokation:
	Rect();
	fRect();
	iRect();
	uRect();
	dRect();

	// Cvt ctors:
//!!?? WTF:
//!!??	fRect(iRect());

	static_assert(is_same_v<decltype(Vec2{1, 2}), Vec2<int>>);

	iRect ir4(Vector_Interface<adapter::Vector2_Impl<int, 2>>(1, 2), Vector_Interface<adapter::Vector2_Impl<int, 2>>(3, 4));
	iRect ir4_2p(iVec2(1, 2), iVec2(3, 4));
	iRect ir4_2b{ Vec2{1, 2},  Vec2{3, 4}};
	iRect ir4_3 = iRect(iVec2(1, 2), iVec2(3, 4));   static_assert(is_same_v<decltype(ir4_3), Rect<int>>);
	iRect ir4_4 = iRect( Vec2(1, 2),  Vec2(3, 4));   static_assert(is_same_v<decltype(ir4_4), Rect<int>>);
	iRect ir4_5 = iRect( Vec2{1, 2},  Vec2{3, 4});   static_assert(is_same_v<decltype(ir4_5), Rect<int>>);
	iRect ir4_6 =      { Vec2{1, 2},  Vec2{3, 4}};   static_assert(is_same_v<decltype(ir4_6), Rect<int>>);

 	auto  irect_copy_1 = ir4;         static_assert(is_same_v<decltype(irect_copy_1), Rect<int>>);
 	auto  irect_copy_2 = iRect(ir4);  static_assert(is_same_v<decltype(irect_copy_2), Rect<int>>);
 	iRect irect_copy_3 = iRect(ir4);  static_assert(is_same_v<decltype(irect_copy_3), Rect<int>>);

	// Ref:
	[[maybe_unused]] iRect& irect_ref = ir4;


	//--------------------------------------------------------------------
	// Converting from different number types...

	iRect const_ir4;
	auto cfr = fRect(const_ir4);    static_assert(is_same_v<decltype(cfr),         Rect<float>>);

#if 1
//	fRect frect_explicit   = Rectangle_Interface<adapter::Rectangle_Impl<float>>(ir4);
	fRect frect_explicit_2 = fRect(ir4); static_assert(is_same_v<decltype(frect_explicit_2), Rect<float>>);
	auto fr0 = fRect();             static_assert(is_same_v<decltype(fr0),         Rect<float>>);

	auto frect_cvt_1 = fRect(ir4);  static_assert(is_same_v<decltype(frect_cvt_1), Rect<float>>);
	// Shouldn't compile due to the `explicit` ctor:
	//fRect frect_cvt_2 = ir4;         static_assert(is_same_v<decltype(frect_cvt_2), Rect<float>>);
	
	// From lval:
	fRect frect_cvt_from_lval_i_p = fRect(ir4);  static_assert(is_same_v<decltype(frect_cvt_from_lval_i_p), Rect<float>>);
	fRect frect_cvt_from_lval_i_b = fRect{ir4};  static_assert(is_same_v<decltype(frect_cvt_from_lval_i_b), Rect<float>>);

//	fRect frect_cvt_from_lval_i_pp(fRect(ir4));  static_assert(is_same_v<decltype(frect_cvt_from_lval_i_pp), Rect<float>>);
	fRect frect_cvt_from_lval_i_pb(fRect{ir4});  static_assert(is_same_v<decltype(frect_cvt_from_lval_i_pb), Rect<float>>);
	fRect frect_cvt_from_lval_i_bp{fRect(ir4)};  static_assert(is_same_v<decltype(frect_cvt_from_lval_i_bp), Rect<float>>);
	fRect frect_cvt_from_lval_i_bb{fRect{ir4}};  static_assert(is_same_v<decltype(frect_cvt_from_lval_i_bb), Rect<float>>);

	// From rval:
//!!?? WTF when explicit:
	fRect frect_cvt_from_rval_i_pp = fRect(iRect()); static_assert(is_same_v<decltype(frect_cvt_from_rval_i_pp), Rect<float>>);
	fRect frect_cvt_from_rval_i_pb = fRect(iRect{}); static_assert(is_same_v<decltype(frect_cvt_from_rval_i_pb), Rect<float>>);
	fRect frect_cvt_from_rval_i_bp = fRect{iRect()}; static_assert(is_same_v<decltype(frect_cvt_from_rval_i_bp), Rect<float>>);
	fRect frect_cvt_from_rval_i_bb = fRect{iRect{}}; static_assert(is_same_v<decltype(frect_cvt_from_rval_i_bb), Rect<float>>);
	fRect frect_cvt_from_rval_i = iRect();           static_assert(is_same_v<decltype(frect_cvt_from_rval_i),    Rect<float>>);

	// Move:
	fRect frect_moved_from_default = fRect(); static_assert(is_same_v<decltype(frect_moved_from_default), Rect<float>>);

	// This should seriously NOT compile:
 	//fRect& firect_ref_mismatch = ir4;
/*
	m_vertices[0].texCoords = sf::Vector2f(rect.left(), rect.top());
	m_vertices[1].texCoords = sf::Vector2f(rect.left(), rect.top() + rect.height());
	m_vertices[2].texCoords = sf::Vector2f(rect.left() + rect.width(), rect.top());
	m_vertices[3].texCoords = sf::Vector2f(rect.left() + rect.width(), rect.top() + rect.height());
*/
#endif
ofstream("run_count.tmp", std::ios_base::app) << "x";
}
