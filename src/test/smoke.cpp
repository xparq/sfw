#include "sfw/GUI.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <iostream>

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
		//!!assert("Setup should fail due to dead host window; although -> #254!")
		return 0; // "Failed successfully", with the dead window.
	}

	return 1; // WTF?! Failed to even fail?!
}
