#ifndef _SPPDXCW345YN879Y6X24UCN7NYH895V6RMUBUBR7IKNB_
#define _SPPDXCW345YN879Y6X24UCN7NYH895V6RMUBUBR7IKNB_


#include <SFML/Window/Event.hpp>

#include <optional> // forward
#include <utility> // forward

#include <cassert>

namespace SAL::event::adapter
{

	struct Input_Impl
	{
	public:
		using native_type = sf::Event;

	private:
		std::optional<native_type> _d;

	public:
		constexpr       auto& native()       { assert(_d); return _d.value(); }
		constexpr const auto& native() const { assert(_d); return _d.value(); }

		// Null instance:
		//!!constexpr  // not with assert() :-/
		Input_Impl() { assert((bool)(*this) == false); }

		// Copy (should also accept native_type):
		Input_Impl(const std::optional<native_type>& other) : _d(other) {}

		// Reuse the native ctors:
//!!??		using native_type::native_type;

		// Reuse the native ctors:
		template <typename... T>
		constexpr Input_Impl(T&&... args) : _d(std::forward<T>(args)...) {}

/*!!
		// Backend event type adapter base
		struct InputType //!!: public sf::Event //!! The SFML category types don't have a common base, actually!
		{
			using native_type = sf::Event;
			//!!...
		};
!!*/
		// Proxy accessors for sf::Event's std::optional API:
		      auto& operator -> ()       { assert(_d); return _d; }
		const auto& operator -> () const { assert(_d); return _d; }

		      auto& value()       { assert(_d); return std::forward<      sf::Event&>(_d.value()); } //!!?? forward is superfluous?
		const auto& value() const { assert(_d); return std::forward<const sf::Event&>(_d.value()); } //!!?? forward is superfluous?

		operator bool() const { return _d.has_value(); }

		template <typename InputT> bool          is()    const { return _d.has_value() ? _d->is<InputT>() : false; }

		//!! Proxied sf:Event -> std::variant, which simply returns null on error! :)
		template <typename InputT> const InputT* getIf() const { return _d->getIf<InputT>(); }

		template <typename InputT> const InputT& get() const { assert(_d); return *(_d->getIf<InputT>()); }

		// Type-specific accessors:
//!!ONLY FOR KeyChanged:
//!!		auto code() { return _d.code(); }


}; // class Input_Impl


} // namespace SAL::event::adapter


#endif // _SPPDXCW345YN879Y6X24UCN7NYH895V6RMUBUBR7IKNB_
