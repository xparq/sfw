//#include "event.hpp"
#include "SAL/Event.hpp"

#include <iostream>
#include <cassert>


//!! Ignored by GCC... :-o
//!!#pragma GCC diagnostic ignored "-Wunused-variable"

int main() {
	using namespace std;
	using namespace SAL::event;


  // Create

	//Event error__Event_is_not_a_type;

	auto e0 = Event();
	//!!?? This fails to compile:
	//!!??auto e1 = Event<WindowResized>({{3,4}, {100u,200u}});
	// Only this form seems to work:
	auto e1 = Event<WindowResized>(3,4, 100u,200u);

	auto e2 = Event<WindowClosed>();

	// Cargo-copy init:
	auto e3 = Event<KeyPressed>(KeyState{.code = 123, .shift = true});
	//!!?? But fails to compile without the type name, even with adding
	//!!?? the u suffix, and even if the init. list is complete:
	//!!??auto e4 = Event<KeyPressed>({123u, true, false, false, false});


  // Access
  {
	if (e1.type == WindowResized) {
		auto* winstate = e1.get_if<WindowResized>();
		assert(winstate);
		auto [width, height] = winstate->size;
		cout << "Event 1: WindowResized, width=" << width << ", height=" << height << "\n";
	}
	if (e2.type == WindowClosed) {
		cout << "Event 2: Quit\n";
	}

	cerr << "Access Event 1 as wrong type; get_if() should return null: "
	     << e1.get_if<WindowClosed>() << '\n';

//!!	cout << "(Just double-checking; access as correct type: "
//!!	     << (bool)e1.get_if<WindowResized>() << ")\n:";

  }


  // Copy
  {
	auto e_copy = e1;
	assert(e_copy.get_if<WindowResized>());
	auto [width, height] = e_copy.get_if<WindowResized>()->size;
	cout << "Copy of Event 1: width=" << width << ", height=" << height << "\n";
  }


  // Compare

	// C++ prevents auto-generating it, so nope, can't reasonably support it... :-(
	//cout << "e1 == e2?" << boolalpha << (e1 == e2) << '\n';


	// Type mismatch is an access violation! (Should assert out in a DEBUG build.)
	//auto& error = e1.get<None>();
}
