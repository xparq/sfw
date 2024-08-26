#ifndef _SDFM80D4S6Y974DYFRMG978G4M7G9ERF80F3456GU3R5U_
#define _SDFM80D4S6Y974DYFRMG978G4M7G9ERF80F3456GU3R5U_


#include "SAL/event/Input.hpp"

//#include <SFML/Window/WindowBase.hpp> // pollEvent, waitEvent etc.
namespace sf { class WindowBase; }


namespace SAL::event
{

	class Source
	{
	public:
		using nativ_type = sf::WindowBase;
	
	private:
 		nativ_type& _d;

	public:
		Source(nativ_type& backend_source) : _d(backend_source) {}

		Input poll();
	//	Input wait(/*!! timeout !!*/);

	}; // class Source

} // namespace SAL::event


#endif // _SDFM80D4S6Y974DYFRMG978G4M7G9ERF80F3456GU3R5U_
