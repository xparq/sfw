#ifndef _SDFM80D4S6Y974DYFRMG978G4M7G9ERF80F3456GU3R5U_
#define _SDFM80D4S6Y974DYFRMG978G4M7G9ERF80F3456GU3R5U_


#include "SAL.hpp"
#include SAL_ADAPTER(event/Input)

#include <SFML/Window/WindowBase.hpp> // pollEvent, waitEvent etc.


namespace SAL::event
{
	//!! LEGACY LEFTOVER EXPERIM. codes for widget updates (or whatnot):

	class Source
	{
	public:
		using nativ_type = sf::WindowBase;
	
 		nativ_type& _d;

	public:
		Source(nativ_type& backend_source) : _d(backend_source) {}


		Input poll() { return _d.pollEvent(); }

	}; // class Source

} // namespace SAL::event


#endif // _SDFM80D4S6Y974DYFRMG978G4M7G9ERF80F3456GU3R5U_
