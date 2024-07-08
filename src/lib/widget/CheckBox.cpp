#include "sfw/widget/CheckBox.hpp"
#include "sfw/Theme.hpp"

#include "SAL/util/diagnostics.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
	using std::max;


namespace sfw
{


CheckBox::CheckBox(bool checked_state):
	m_box(Box::Input)
{
	set(checked_state);

	onThemeChanged(); //!!kludge to force geom. recalc.!
}

//!!C++: Nope...: CheckBox::CheckBox(Event::Callback& callback, bool checked/* = false*/):
CheckBox::CheckBox(std::function<void(CheckBox*)> callback, bool checked):
	CheckBox(checked)
{
	setCallback(callback);
}

//!!/* See #339, why this couldn't be added...
CheckBox::CheckBox(std::function<void()> callback, bool checked):
	CheckBox(checked)
{
	setCallback(callback);
}
//!!*/

CheckBox* CheckBox::set(bool checked)
{
	m_checked = checked;
	setChanged();
	return this;
}


void CheckBox::draw(const gfx::RenderContext& ctx) const
{
	auto lctx = ctx;
	lctx.props.transform *= getTransform();
	m_box.draw(lctx);
	if (checked())
		m_checkmark.draw(lctx);
}


// callbacks -------------------------------------------------------------------

void CheckBox::onActivationChanged(ActivationState state)
{
	m_box.applyState(state);
}


void CheckBox::onThemeChanged()
{
	float chkmark_offset = Theme::PADDING + Theme::borderSize;
	float box_size = m_checkmark.getSize().x() + chkmark_offset * 2;
	m_box.setSize(box_size, box_size);
	setSize(max(box_size, (float)Theme::getLineSpacing() / 1.5f),
	        max(box_size, (float)Theme::getBoxHeight()) * 0.85f); //!! Heuristic quick-fix for #199...

/*!! OLD:
	m_box.setPosition((getSize().x - m_box.getSize().x) / 2,
	                  (getSize().y - m_box.getSize().y) / 2);
	m_checkmark.setPosition({m_box.getPosition().x + chkmark_offset,
	                         m_box.getPosition().y + chkmark_offset});
!!*/				 
	m_box.setPosition((getSize() - m_box.getSize()) / 2);
	m_checkmark.setPosition(m_box.getPosition() + chkmark_offset);
}


void CheckBox::onMouseReleased(float x, float y)
{
	if (contains(sf::Vector2f(x, y)))
	{
		toggle();
	}
}


void CheckBox::onKeyPressed(const sf::Event::KeyChanged& key)
{
	if (key.code == sf::Keyboard::Key::Space)
	{
		toggle();
	}
}


} // namespace sfw
