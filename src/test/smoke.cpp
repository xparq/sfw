#include "sfw/GUI.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <iostream>
#include <cassert>
using namespace std;

int main()
{
	//------------------------------------------------------------------------
	// Setup low-level GFX (GL + X (or Windows) context
	sf::RenderWindow dummy_window;

	//------------------------------------------------------------------------
	// Setup the Test GUI...
	using namespace sfw;
	GUI demo(dummy_window, Theme::DEFAULT, false); // false: don't own the window
	if (!demo) {
		return 0; // Failure is expected here for now (with a dead window...)!
	}

	return 1; // WTF?! Failed to even fail?!
}
