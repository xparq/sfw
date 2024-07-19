#include "SAL/Event.hpp"
#include "sfw/GUI-main.hpp"

#include <SFML/Window.hpp>  // app window & the system (SFML) event queue

//#include <type_traits> // is_same
#include <iostream>
#include <thread> // sleep
#include <fstream> // Just for updating the run-count file! :)

using namespace std;
using namespace SAL;


//----------------------------------------------------------------------------
// Values...

int main() try
{
	//------------------------------------------------------------------------
	// Normal SFML app-specific setup....

	// Create the main window, as usual for any SFML app
	sf::RenderWindow window;
	window.create(sf::VideoMode({1024, 768}), "SFW Demo", sf::Style::Close|sf::Style::Resize);
	if (!window.isOpen()) {
		cerr << "- Failed to create the SFML window!\n";
		return EXIT_FAILURE;
	}
	window.setFramerateLimit(30);


	sfw::GUI demo(window);
	if (!demo) {
		return EXIT_FAILURE; // Errors have already been printed to cerr.
	}


	//--------------------------------------------------------------------
	// Event Loop
	//--------------------------------------------------------------------
//cerr<<"Starting the event loop..."<< endl;
	while (demo)
	{
		// While somewhat less elegant than the textbook double-loop solution,
		// this unrolled single-loop structure (not counting the raw-mouse-move
		// filter loop) is chosen for allowing screen updates both after input events
		// and also after some idle timeout (to support showing (polling) changes
		// made by other threads), without either awkwardly duplicating the updates
		// in a nested event-puming loop or having an initial extra draw outside
		// the main loop. (-> FizzBuzz?... ;) )

		//SAL::Event::Source event_source(window);
		//SAL::event::Input event = event_source.poll();
		//SAL::event::Input event = demo.poll();
		auto event = demo.poll();

		while (event && event->is<sf::Event::MouseMovedRaw>()) event = window.pollEvent(); // Ignore the raw mouse-move event spam of SFML3! :-/
		if (event)
		{
			// Pass the event to the GUI:
			demo.process(event);

			// Close on Esc:
			if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
				demo.close();

			if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space)
				cout << "SPACE" << '\n';
		}

		// Redraw the UI into the rendering (frame) buffer of the connected (or some other) window:
		demo.render(/*!!maybe_to_some_other_window!!*/);
		// Show the updated window:
		window.display();

		if (!event) this_thread::sleep_for(20ms);
	}


ofstream("run_count.tmp", std::ios_base::app) << "x";
}
catch(...)
{
	cerr << "- ERROR: UNHANDLED EXCEPTION!" << endl;
}
