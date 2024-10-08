#include "sfw/widget/Button.hpp"
#include "sfw/Theme.hpp"

#include <algorithm>
	using std::max;


namespace sfw
{

Button::Button(const std::string_view text)
{
	onThemeChanged(); //!!Calling it this way is a temp. kludge (for DRY). Also: it has to happen before the rest of the init.
	setText(text); // Will resize, too
}

Button::Button(const std::string_view text, std::function<void()> callback):
	Button(text)
{
	setCallback(callback);
}

Button::Button(const std::string_view text, std::function<void(Button*)> callback):
	Button(text)
{
	setCallback(callback);
}


Button* Button::setText(std::string_view text)
{
	static_assert(std::is_convertible_v<decltype(m_box.item()), gfx::Text>); // Just in case I ever change it... ;)
	m_box.item().set(text);
	recomputeGeometry();
	return this;
}

std::string Button::getText() const
{
	static_assert(std::is_convertible_v<decltype(m_box.item()), gfx::Text>); // Just in case I ever change it... ;)
	return m_box.item().get();
}


Button* Button::setColor(Color c)
{
	m_box.setTintColor(c);
	return this;
}


Button* Button::setTextColor(Color c)
{
	static_assert(std::is_convertible_v<decltype(m_box.item()), gfx::Text>); // Just in case I ever change it... ;)
	m_box.setItemColor(c);
	return this;
}

Button* Button::click()
{
	//! This weird kludge won't do antything to the visuals. (That would require
	//! way more hocus pocus, for just a little gimmick, so... perhaps later.)
	setChanged();
	updated();
	return this;
}


// ---------------------------------------------------------------------------

void Button::onThemeChanged()
{
	static_assert(std::is_convertible_v<decltype(m_box.item()), gfx::Text>); // Just in case I ever change it... ;)
	m_box.item().font(Theme::getFont());
	m_box.item().font_size(Theme::fontSize);
	m_box.setSize((float)Theme::minWidgetWidth, Theme::getBoxHeight());
	recomputeGeometry();
}


void Button::recomputeGeometry() // override
{
	int fittingWidth = (int)(m_box.contentSize().x() + Theme::PADDING * 2 + Theme::borderSize * 2);
	int width = max(fittingWidth, Theme::minWidgetWidth);

	m_box.setSize((float)width, Theme::getBoxHeight());
	setSize(m_box.getSize());

	static_assert(std::is_convertible_v<decltype(m_box.item()), gfx::Text>); // Just in case I ever change it... ;)
	m_box.item().center_in(getRect());
}


void Button::draw(const gfx::RenderContext& ctx) const
{
	auto lctx = ctx;
	lctx.props.transform *= getTransform(); //!! DIRECT SFML USE
	m_box.draw(lctx);
}

// Callbacks -------------------------------------------------------------------

void Button::onActivationChanged(ActivationState state)
{
	m_box.applyState(state);
}


void Button::onMouseMoved(float x, float y)
{
	if (getActivationState() == ActivationState::Pressed)
	{
		if (contains(sf::Vector2f(x, y)))
			m_box.press();
		else
			m_box.release();
	}
}


void Button::onMousePressed([[maybe_unused]] float x, [[maybe_unused]] float y)
{
	m_box.press();
}


void Button::onMouseReleased(float x, float y)
{
	if (contains({x, y}))
	{
		onUpdated();
	}
}


void Button::onKeyPressed(const event::KeyCombination& key)
{
	if (key.code == unsigned(sf::Keyboard::Key::Enter) || key.code == unsigned(sf::Keyboard::Key::Space)) //!!XLAT
	{
		m_box.press();
		onUpdated();
	}
}


void Button::onKeyReleased(const event::KeyCombination& key)
{
	if (key.code == unsigned(sf::Keyboard::Key::Enter)) //!!XLAT
	{
		m_box.release();
	}
}


} // namespace sfw
