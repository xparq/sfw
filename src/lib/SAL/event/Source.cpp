#include "SAL/event/Source.hpp"

#include <SFML/Window/WindowBase.hpp>
#include <SFML/Window/Event.hpp>


namespace SAL::event
{

Input Source::poll()
{
	const auto& in = _d.pollEvent();

	if (in) {
		if (in->is<sf::Event::MouseMoved>())
		{
			const auto& mouse = in->getIf<sf::Event::MouseMoved>();
			return Event<MouseMoved>(MousePosition{
				.position = {mouse->position.x, mouse->position.y},
				.raw_delta = {0,0}
			});
		}
		else if (in->is<sf::Event::MouseMovedRaw>())
		{
			const auto& mouse = in->getIf<sf::Event::MouseMovedRaw>();
			return Event<MouseMovedRaw>(MousePosition{
				.position = {0,0},
				.raw_delta = {mouse->delta.x, mouse->delta.y}
			});
		}
		else if (in->is<sf::Event::MouseWheelScrolled>())
		{
			const auto& mouse = in->getIf<sf::Event::MouseWheelScrolled>();
			return Event<MouseWheelMoved>(MouseWheelInfo{
				.wheel = event::u16(mouse->wheel),
				.delta = mouse->delta,
				.position = {mouse->position.x, mouse->position.y},
			});
		}
		else if (in->is<sf::Event::MouseButtonPressed>())
		{
			const auto& mouse = in->getIf<sf::Event::MouseButtonPressed>();
			return Event<MouseButtonPressed>(MouseButtonInfo{
				event::u16(mouse->button), //!! XLAT
				{mouse->position.x, mouse->position.y}
			});
		}
		else if (in->is<sf::Event::MouseButtonReleased>())
		{
			const auto& mouse = in->getIf<sf::Event::MouseButtonReleased>();
			return Event<MouseButtonReleased>(MouseButtonInfo{
				event::u16(mouse->button), //!! XLAT
				{mouse->position.x, mouse->position.y}
			});
		}
		else if (in->is<sf::Event::KeyPressed>())
		{
			const auto& key = in->getIf<sf::Event::KeyPressed>();
			return Event<KeyPressed>(KeyState{
				.code = unsigned(key->code), //!! Fukit, SFML3 made it an enum class. :-/
//!!				.code = reinterpret_cast<unsigned>(key->code), //!! Fukit, SFML3 made it an enum class. :-/
//				.scancode = key->scancode,
				.alt = key->alt,
				.control = key->control,
				.shift = key->shift,
				.system = key->system
			});
		}
		else if (in->is<sf::Event::TextEntered>())
		{
			const auto& text = in->getIf<sf::Event::TextEntered>();
			return Event<TextEntered>(TextInput{
			        .codepoint = text->unicode
			});
		}
		else if (in->is<sf::Event::Closed>())
		{
			return Event<WindowClosed>(empty_t{});
		}
		// Else:
//!!		return Input(in);
		return Event();
	}

	// Else:
	return Input(); // Empty (false)
}

} // namespace SAL::event
