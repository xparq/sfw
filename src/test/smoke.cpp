#include "sfw/GUI.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <iostream>
#include <cassert>
using namespace std;

int main()
{
	//------------------------------------------------------------------------
	// Setup the Test GUI...
	using namespace sfw;

/*!! Even this crashes with "Failed to open X11 display; make sure the DISPLAY
   environment variable is set correctly" 
	sf::RenderWindow dummy_window;
	GUI demo(dummy_window, Theme::DEFAULT, false); // false: don't own the window
	if (!demo) {
		return 0; // Failure is expected here for now...
	}
!!*/
	return 0;
}
