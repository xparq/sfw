#ifndef _SPPDXCW345YN879Y6X24UCN7NYH895V6RMUBUBR7IKNB_
#define _SPPDXCW345YN879Y6X24UCN7NYH895V6RMUBUBR7IKNB_


#include "SAL/event/Keyboard.hpp"
#include "SAL/math/Vector.hpp" //!! But... Using vectors breaks the C++ "triviality" of the Event payload class! :-(

#include <utility> // forward
#include <cstdint> // int32_t, ...

//!! Should perhaps be controlled with a define to disable including the heaviy <type_traits>!
// For supporting untagged Event{type, {...}); init
#include <initializer_list>
#include <type_traits>

#include <cassert>


namespace SAL::event
{

//----------------------------------------------------------------------------
// Event types
//----------------------------------------------------------------------------
	enum Type {
		//--------------------------------------------------------------------
		//!!
		//!! MOVE TO Input(_Interface)!
		//!!
		//!! Try to come up with a disjunct categorization, similar to, but sg. less ambiguous than:
		//	User inputs (e.g. SensorChange is also an input (as in fact everything...), but not from the user. Well, not /directly/..., but...)
		//	Notifications about higher level actions (rather than direct user inputs)
		//	- UI events, coming from the host!
		//	System events (But... e.g. SensorChange...? :) )
		//!!
		//!! Also: integrate (intuitively) with the internal SFW events! :-o A good design is especially
		//!! critical for clean interop with the host/system UI (e.g. windowing system)!
		//!!
		//!! Stop using past participles here: adds nothing, but is awkwardly forced and unnatural!
		//!! (Follow e.g. the Win32 names, where they are good.)
		//!! But these names still need some differentiating trait from other events (like a prefix/suffix (lame), and/or a subtype)!
		//!!
		//--------------------------------------------------------------------

		None = 0, // I'd rather die than us `std::monostate` with std::variant. ;-p

		WindowClosed,
		WindowResized,
		WindowMoved,
		WindowFocused,
		WindowUnfocused,
		WindowMinimized,
		WindowDocked,
		WindowRestored, //!! May not be a viable pair of both Docked & Minimized!

		KeyPressed,
		KeyReleased,
		TextEntered,

		MouseMoved,
		MouseMovedRaw,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseWheelMoved,
		MouseMovedIn,
		MouseMovedOut,

		ControllerButtonPressed,
		ControllerButtonReleased,
		ControllerStickMoved,
		ControllerConnected,
		ControllerDisconnected,

		TouchStarted,
		TouchMoved,
		TouchEnded,

		SensorChanged,

		//ClockTick
		//Timer
		//Error
#ifdef DEBUG
		_COUNT_
#endif
	};

//----------------------------------------------------------------------------
// Event payload types
//----------------------------------------------------------------------------

	using i64  = std::int64_t;
	using u64  = std::uint64_t;
	using i32  = std::int32_t;
	using u32  = std::uint32_t;
	using i16  = std::int16_t;
	using u16  = std::uint16_t;
	using i8   = std::uint8_t;
	using u8   = std::uint8_t;
//	using byte = std::uint8_t;

	struct ivec2 { i32 x; i32 y; };
	struct uvec2 { u32 x; u32 y; };
	struct fvec2 { float x; float y; };
	struct fvec3 { float x; float y; float z; };

		//----------------------------------------
		struct empty_t {
		};

		//!! Actually, most of of these should contain an ID to identify the source of
		//!! the event!... Remember that even mice and keyboards can be more than one
		//!! attached simultaneously (and working seamlessy e.g. on Windows, so that
		//!! could be a nice and easy way to support multiple players -- except neither
		//!! SFML nor Windows tell in the event data which device was the source. :-/

		//----------------------------------------
		struct WindowState {
			ivec2 position;   // Position, in pixels
			uvec2 size;       // Size, in pixels
		};

		//----------------------------------------
		struct MousePosition {
			ivec2 position;  // Position of the mouse pointer, relative to the top left of the owner window
			ivec2 raw_delta; // Distance the mouse moved since the last time
		};
		struct MouseButtonInfo {
			u16   button;    // ID of the pressed/released button
			ivec2 position;  // (See MousePosition)
		};
		struct MouseWheelInfo {
			u16   wheel;              // Index of the wheel (some mice have more than one).
			float delta;           // Wheel offset (positive is up/left, negative is down/right). High-precision mice may use non-integral offsets.
			ivec2 position;  // (See MousePosition)
		};
		/*!! The `...State` approach would make prop. paths very awkward in the client code, like
		//!! `mousemoved.position.position` or `.button.button', or '.button.position`, or
		//!! `.wheel.position` meaning the mouse position it the WheelInfo struct... *Sigh.*
		//!! Multiple inheritance could eliminate that -- at the cost of very weird-looking code...
		//!! And it might still require naming the properties carefully, and unique across sub-structs.
		//!! It could be much better to ONLY have a ...State struct then. But it's more difficult to
		//!! design than the individual (but then arbitrarily limited) ones. They are at least faster
		//!! to setup and pass around.
		struct MouseState {
			MousePosition   position;
			MouseButtonInfo button;
			MouseWheelInfo  wheel;
		};
		*/
		//----------------------------------------
		struct ControllerInfo {
			u16 id;          // Index of the device (e.g. a game controller)
			u16 n_buttons;   // Number of buttons on the device
			u16 n_sticks;    // Number of joysticks on the device
		};
		struct ControllerUpdate {
			u16   id;    // Index of the device (e.g. a game controller)
			u16   button;    // Index of the button that has been pressed
			u16   axis;      // Axis along which the joystick moved
			float position;  // New position on the axis (in range [-100.0 .. 100.0])
		};

		//----------------------------------------
		struct TouchState {
			u16    finger;   // Index of the finger (with multi-touch events it can be > 1)
			ivec2  position; // Position of the touch, relative to the top left of the owner window
		};

		//----------------------------------------
		struct SensorState {
			u16      id;      // Index of the sensor
			u32      type;    //!! (e.g. Compass)
			fvec2    data_2d;
//!!			fvec3    data_3d;
		};
		//----------------------------------------
//!!		struct SensorState2D {
//!!			unsigned         sensor;      // Index of the sensor
//!!			sf::Sensor::Type type;        //!! (e.g. Compass)
//!!			fVec2            value;
//!!		};
//!!		struct SensorState3D {
//!!			unsigned         sensor;      // Index of the device
//!!			sf::Sensor::Type type;        //!! (e.g. Accelerometer)
//!!			fVec3            value;
//!!		};


//------------------------------------------------------------------------------------
// Internals...
//------------------------------------------------------------------------------------
//!!namespace internal
//{
	//----------------------------------------------------------------------------
	// Event type -> cargo type map
	//----------------------------------------------------------------------------
	template<Type E> struct PayloadType;
#define _M_DEF(ET, PT) template<> struct PayloadType<ET> { using type = PT; }
	_M_DEF(None,                empty_t);

	_M_DEF(KeyPressed,          KeyState);
	_M_DEF(KeyReleased,         KeyState);
	_M_DEF(TextEntered,         TextInput);
	_M_DEF(MouseMoved,          MousePosition);
	_M_DEF(MouseMovedIn,        empty_t);
	_M_DEF(MouseMovedOut,       empty_t);
	_M_DEF(MouseMovedRaw,       MousePosition);
	_M_DEF(MouseButtonPressed,  MouseButtonInfo);
	_M_DEF(MouseButtonReleased, MouseButtonInfo);
	_M_DEF(MouseWheelMoved,     MouseWheelInfo);
	_M_DEF(ControllerButtonPressed,  ControllerUpdate);
	_M_DEF(ControllerButtonReleased, ControllerUpdate);
	_M_DEF(ControllerStickMoved,     ControllerUpdate);
	_M_DEF(ControllerConnected,      ControllerInfo);
	_M_DEF(ControllerDisconnected,   ControllerInfo);
	_M_DEF(TouchStarted,        TouchState);
	_M_DEF(TouchMoved,          TouchState);
	_M_DEF(TouchEnded,          TouchState);
	_M_DEF(SensorChanged,       SensorState);

	_M_DEF(WindowClosed,        empty_t);
	_M_DEF(WindowResized,       WindowState);
	//_M_DEF(WindowMoved,         WindowState);
	_M_DEF(WindowFocused,       empty_t);
	_M_DEF(WindowUnfocused,     empty_t);
	//_M_DEF(WindowMinimized,     empty_t);
	//_M_DEF(WindowDocked,        empty_t);
	//_M_DEF(WindowRestored,      WindowState);

	// Non-user-originated ("system") events...

	//_M_DEF(ClockTick,           TimeInfo);
	//_M_DEF(Timer,               TimeInfo);

	//_M_DEF(Error,               ErrorInfo);

#undef _M_DEF

	//----------------------------------------------------------------------------
	// The unified Event (data) struct (as a bare tagged union)
	//----------------------------------------------------------------------------
	struct EventData {
		Type type;
		union {
		private:// Can't make the entire payload private, as the factory template
			// functions need access! But we do have this fun little lucky
			// chance here to still just lock out exactly the "dangerous" bits.
			//
			// Note: the member names are not user-facing (or in fact: anyone-
			//       facing), so they're just dummy id's to keep C++ alive...
			empty_t                  _empty_t;
			WindowState        	 _WindowState;
			KeyState        	 _KeyState;
			TextInput        	 _TextInput;
			MousePosition            _MousePosition;
			MouseButtonInfo          _MouseButtonInfo;
			MouseWheelInfo           _MouseWheelInfo;
			//MouseState               _MouseState;
			ControllerInfo           _ControllerInfo;
			ControllerUpdate         _ControllerUpdate;
			TouchState               _TouchState;
			SensorState              _SensorState;
		} payload;

		//--------------------------------------------------------------------
		// Check if it's a non-empty type
		constexpr
		operator bool () const { return type != None; }

		//--------------------------------------------------------------------
		// Check if it's of a specific type
		template<Type E> constexpr bool
		is() const { return type == E; }

		//--------------------------------------------------------------------
		// Get as a specific type
		// - You must always check for the type first!
		// - Accessing as a different type is considered an access violation!
		template<Type E> constexpr const PayloadType<E>::type*
		get_if() const;

		//--------------------------------------------------------------------
		// Wishful thinking!... C++ refuses to auto-generate it for the union!
		//bool operator == (const EventData& other) const = default;
	}; // EventData

#ifdef DEBUG
	static_assert(std::is_copy_constructible_v<EventData>);
	static_assert(std::is_copy_assignable_v<EventData>);
	static_assert(std::is_trivial_v<EventData>);
#endif


//!!} // anon. namespace

//----------------------------------------------------------------------------
// Factory functions (pretending to be the event ctor...)
//----------------------------------------------------------------------------

// Default (null) init (non-const to allow assignments)
constexpr auto
Event()
{
	EventData e;
	e.type = None;
	return e;
}

// Init with direct in-place payload params.: Event(WindowResized, 100, 200)
// -- NOT suitable for untagged aggreg. init lists: Event(e, {some, data...})
//    But DOES handle tagged ones: Event(e, MyPayload{a, b...})
template<Type E, typename... Args> constexpr auto // const just to prevent assignment!
Event(Args&&... args)
{
	EventData e;
	e.type = E;
	using PayloadT = typename PayloadType<E>::type;
	new (&e.payload) PayloadT{std::forward<Args>(args)...};
	return e;
}

/*!!?? -- Not needed? See the variadic one!
// Init with payload copy
// ...doesn't need its own "constructor". (OTOH, even this won't help with
// supporting untagged (bare) (..., {x, y}) initializer lists! That won't work. :(
template<Type E, typename PayloadT> constexpr auto const // const just to prevent assignment!
Event(PayloadT&& arg)
{
	EventData e;
	e.type = E;
	new (&e.payload) PayloadT{std::forward<decltype(arg)>(arg)};
	return e;
}
??!!*/


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!! Just for the migration:
using Input_Impl = EventData;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


//----------------------------------------------------------------------------
// EventData member impl.
//----------------------------------------------------------------------------
//namespace // Can't hide it, as that would "infect" the users of event::Input (which EventData currently is...) with internal linkage! :-o
//{
	template<Type E> constexpr const PayloadType<E>::type*
	EventData::get_if() const {
		if (type != E) // This is already access violation (UB), so anything goes...
			return nullptr;
		return reinterpret_cast<const PayloadType<E>::type*>(&payload);
	}

/*!!OLD:
	constinit static inline const auto Null = Event();

	template<Type E> constexpr const PayloadType<E>::type&
	EventData::get_if() const {
		assert(type == E);
		if (type != E) // This is already access violation (UB) territory, so anything goes!... ;)
			return reinterpret_cast<const PayloadType<E>::type&>(Null.payload);
		return reinterpret_cast<const PayloadType<E>::type&>(payload);
	}
!!*/

//} // anon. namespace


} // namespace SAL::event


#endif // _SPPDXCW345YN879Y6X24UCN7NYH895V6RMUBUBR7IKNB_
