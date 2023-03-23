#include "sfw/Widgets/ProgressBar.hpp"
#include "sfw/Theme.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace sfw
{

ProgressBar::ProgressBar(float length, Orientation orientation, LabelPlacement labelPlacement):
    m_orientation(orientation),
    m_boxLength(length),
    m_labelPlacement(labelPlacement),
    m_value(0.f),
    m_box(Box::Input)
{
    setValue(m_value);
    setSelectable(false);

    updateGeometry();
}


ProgressBar* ProgressBar::setValue(float value)
{
    m_value = value;

    updateGeometry();

    return this;
}


float ProgressBar::getValue() const
{
    return m_value;
}


void ProgressBar::draw(const gfx::RenderContext& ctx) const
{
    auto sfml_renderstates = ctx.props;
    sfml_renderstates.transform *= getTransform();
    ctx.target.draw(m_box, sfml_renderstates);
    sfml_renderstates.texture = &Theme::getTexture();
    ctx.target.draw(m_bar, 4, sf::PrimitiveType::TriangleStrip, sfml_renderstates);
    if (m_labelPlacement != LabelNone)
        ctx.target.draw(m_label, sfml_renderstates);
}


void ProgressBar::onThemeChanged()
{
    updateGeometry();
}

void ProgressBar::updateGeometry()
{
    // Bar...

    if (m_orientation == Horizontal)
    {
        m_box.setSize(m_boxLength, Theme::getBoxHeight());
    }
    else
    {
        m_box.setSize(Theme::getBoxHeight(), m_boxLength);
        if (m_labelPlacement == LabelOver)
            m_label.setRotation(sf::degrees(90.f));
    }

    m_label.setString("100%");
    m_label.setFont(Theme::getFont());
    m_label.setFillColor(Theme::input.textColor);
    m_label.setCharacterSize((unsigned)Theme::textSize);

    // Build bar
    const float x1 = Theme::PADDING;
    const float y1 = Theme::PADDING;
    const float x2 = (m_orientation == Horizontal ? m_boxLength : Theme::getBoxHeight()) - Theme::PADDING;
    const float y2 = (m_orientation == Horizontal ? Theme::getBoxHeight() : m_boxLength) - Theme::PADDING;
    m_bar[0].position = {x1, y1};
    m_bar[1].position = {x1, y2};
    m_bar[2].position = {x2, y1};
    m_bar[3].position = {x2, y2};

    sf::FloatRect rect = (sf::FloatRect)Theme::getProgressBarTextureRect();
    m_bar[0].texCoords = sf::Vector2f(rect.left, rect.top);
    m_bar[1].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
    m_bar[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
    m_bar[3].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);

    float labelWidth = m_label.getLocalBounds().width;
    float labelHeight = m_label.getLocalBounds().height;
    if (m_labelPlacement == LabelOutside)
    {
        if (m_orientation == Horizontal)
        {
            // Place label on the right of the bar
            m_label.setPosition({m_boxLength + Theme::PADDING, Theme::PADDING});
            setSize({m_boxLength + Theme::PADDING + labelWidth, m_box.getSize().y});
        }
        else
        {
            // Place label below the bar
            setSize({m_box.getSize().x, m_boxLength + Theme::PADDING + labelHeight});
        }
    }
    else
    {
        setSize(m_box.getSize());
    }

    // Label...

    m_label.setString(std::to_string((int)m_value) + "%");
    if (m_orientation == Horizontal)
    {
        float x = Theme::PADDING + (m_box.getSize().x - Theme::PADDING * 2) * m_value / 100;
        m_bar[2].position.x = m_bar[3].position.x = x;
        if (m_labelPlacement == LabelOver)
        {
            m_box.centerTextHorizontally(m_label);
        }
    }
    else
    {
        float fullHeight = m_box.getSize().y - Theme::PADDING * 2;
        float y = fullHeight * m_value / 100;
        m_bar[0].position.y = m_bar[2].position.y = (fullHeight - y) + Theme::PADDING;
        if (m_labelPlacement == LabelOver)
        {
            m_box.centerVerticalTextVertically(m_label);
        }
        else if (m_labelPlacement == LabelOutside)
        {
            // Re-center label horizontally (text width can change)
            float labelX = (m_box.getSize().x - m_label.getLocalBounds().width) / 2;
            m_label.setPosition({labelX, m_box.getSize().y + Theme::PADDING});
        }
    }
}

} // namespace
