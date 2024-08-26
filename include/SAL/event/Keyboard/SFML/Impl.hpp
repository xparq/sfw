#ifndef _K3LLSDFDCM40389MY2D4097MEY60978HC98U5VMHBG_
#define _K3LLSDFDCM40389MY2D4097MEY60978HC98U5VMHBG_


namespace SAL::event//!!::adapter//!!::SFML
{
	//----------------------------------------
	struct KeyState {
		unsigned      code;     // Label of the pressed/released key
//		sf::Keyboard::Scancode scancode; // Physical code of the key that has been pressed
		bool                   alt;      // Is the Alt key pressed?
		bool                   control;  // Is the Control key pressed?
		bool                   shift;    // Is the Shift key pressed?
		bool                   system;   // Is the System key pressed?

		constexpr bool operator==(const KeyState& other) const = default; // C++20
/*!!OLD:		{
			return   code     == other.code
				//&& scancode == other.scancode 				
				&& alt      == other.alt
				&& control  == other.control
				&& shift    == other.shift
				&& system   == other.system
			;
		}
!!*/
		constexpr bool has_modifier() const
		{
			return alt
				|| control
				|| shift
				|| system
			;
		}
	};



//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!! DEPRECATED:
//!!
#if 0
	//---------------------------------------------------------------------------------------
	struct KeyCombination //!! This abomination is the direct result of SFML3 removing the
				//!! KeyChanged event, because it became incompatible with their
				//!! new retro-hipster C++-oriented API design... :-/
	//---------------------------------------------------------------------------------------
	{
		sf::Keyboard::Key      code{};     //!< Code of the key that has been pressed
		sf::Keyboard::Scancode scancode{}; //!< Physical code of the key that has been pressed
		bool                   alt{};      //!< Is the Alt key pressed?
		bool                   control{};  //!< Is the Control key pressed?
		bool                   shift{};    //!< Is the Shift key pressed?
		bool                   system{};   //!< Is the System key pressed?

		// "Zero-init"
//			constexpr KeyCombination() = default;

		// Copy
		constexpr KeyCombination(const KeyCombination&) = default;
		constexpr KeyCombination& operator = (const KeyCombination&) = default;

		// Convert from native...
		constexpr KeyCombination(const sf::Event::KeyPressed& native) :
				code{native.code},
				scancode{native.scancode},
				alt{native.alt},
				control{native.control},
				shift{native.shift},
				system{native.system}
			{}
		constexpr KeyCombination(const sf::Event::KeyReleased& native) :
				code{native.code},
				scancode{native.scancode},
				alt{native.alt},
				control{native.control},
				shift{native.shift},
				system{native.system}
			{}

		// Convert to native...

/*!!?? WTF was wrong with the desig. init?! :-o
		constexpr operator sf::Event::KeyPressed () const { return sf::Event::KeyPressed{
				.code = code,
				.scancode = scancode,
				.alt = alt,
				.control = control,
				.shift = shift,
				.system = system,
			}; }

		constexpr operator sf::Event::KeyReleased () const { return sf::Event::KeyReleased{
				.code = code,
				.scancode = scancode,
				.alt = alt,
				.control = control,
				.shift = shift,
				.system = system,
			}; }
??!!*/			constexpr operator sf::Event::KeyPressed () const { sf::Event::KeyPressed key;
				key.code = code;
				key.scancode = scancode;
				key.alt = alt;
				key.control = control;
				key.shift = shift;
				key.system = system;
				return key;}
		constexpr operator sf::Event::KeyReleased () const { sf::Event::KeyReleased key;
				key.code = code;
				key.scancode = scancode;
				key.alt = alt;
				key.control = control;
				key.shift = shift;
				key.system = system;
				return key;}

		constexpr bool operator==(const KeyCombination& other) const
		{
			return   code     == other.code
				//&& scancode == other.scancode 				
				&& alt      == other.alt
				&& control  == other.control
				&& shift    == other.shift
				&& system   == other.system
			;
		}

		constexpr bool has_modifier() const
		{
			return alt
				|| control
				|| shift
				|| system
			;
		}
	}; // struct KeyCombination
#endif


} // namespace SAL::event//!!::adapter//!!::SFML


#endif // _K3LLSDFDCM40389MY2D4097MEY60978HC98U5VMHBG_
