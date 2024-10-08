﻿#ifndef _WCOVIREHJ5Y9UHW4C9UYH89U7VHME87RCMTYVE5B8TUYTYBJB_
#define _WCOVIREHJ5Y9UHW4C9UYH89U7VHME87RCMTYVE5B8TUYTYBJB_


#include "SAL/event/Input.hpp"

#include <functional>
#include <optional>
#include <variant>
#include <unordered_map>


namespace SAL::event
{
	class Handler;
	namespace internal
	{
		using Callback_void = std::optional<std::function<void()>>;
		using Callback_w    = std::optional<std::function<void(Handler*)>>;

	//!!??	Does this make any measurable difference? (Likely even just using plain ordered map wouldn't.)
	//!!	Note also that this would put every single handler in its own distinct hash bucket! Sounds like
	//!!	a pretty blatant memory overhead, at the very least.)
		template <typename T> struct IDHash
		{
			std::size_t operator()(const T& id) const { return static_cast<std::size_t>(id); }
		};
	}

	using Callback = std::variant<internal::Callback_w, internal::Callback_void>;
	using CallbackMap = std::unordered_map<ID, Callback, internal::IDHash<ID>>;
//!!??	using CallbackMap = std::unordered_map<ID, Callback>; // (see comment above at IDHash)


	//--------------------------------------------------------------------
	// Base class for entites that want to handle sfw::Events
	//--------------------------------------------------------------------
	class Handler // mixin, not for polymorphism via Handler* pointers!
	{
	protected:
		CallbackMap m_callbackMap; // See Widget::on(event::ID, event::Callback)!

		void on(event::ID e, event::Callback f) { m_callbackMap[e] = f; }

		// General, "raw" events: user inputs, system events etc.
		virtual void onMouseMoved(float, float) {}
		virtual void onMousePressed(float, float) {}
		virtual void onMouseReleased(float, float) {}
		virtual void onMouseWheelMoved(int) {}
		virtual void onMouseEnter() {}
		virtual void onMouseLeave() {}
		virtual void onKeyPressed(const KeyCombination&) {}
		virtual void onKeyReleased(const KeyCombination&) {}
		virtual void onTextEntered(char32_t) {}
		virtual void onTick() {}
	};

} // namespace SAL::event


#endif // _WCOVIREHJ5Y9UHW4C9UYH89U7VHME87RCMTYVE5B8TUYTYBJB_
