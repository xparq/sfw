#ifndef GUI_OPTIONSBOX_HPP
#define GUI_OPTIONSBOX_HPP

#include "sfw/Widget.hpp"
#include "sfw/Gfx/Elements/Text.hpp"
#include "sfw/Gfx/Elements/Arrow.hpp"
#include "sfw/Gfx/Elements/ItemBox.hpp"

#include <string>

namespace sfw
{

/**
 * Widget for selecting one item from a list.
 * Callback is triggered when selection is changed.
 */
template <class T>
class OptionsBox: public Widget
{
public:
    OptionsBox();
    OptionsBox(std::function<void(OptionsBox<T>*)> callback);

    /**
     * Append a new item in the list
     * @param label: displayed label when selected
     * @param value: value associated with that label
     */
    auto add(const std::string& label, const T& value);

    /**
     * Update an existing item
     */
    auto set(const std::string& label, const T& value);

    /**
     * Make an item at the specified index the current one
     * (The indexing starts with 0.)
     */
    auto select(size_t index);

    /**
     * Make the item with the specified label the current one
     */
    auto select(const std::string& label);

    /**
     * Get the value of the selected item
     * @return associated value/reference
     */
    const T& current() const;
          T& currentRef();

    /**
     * Get the index of the selected item
     */
    size_t currentIndex() const;

    const std::string& currentLabel() const;

    auto selectNext();
    auto selectPrevious();

    OptionsBox<T>* setTextColor(const sf::Color& color);
    OptionsBox<T>* setFillColor(const sf::Color& color);

    // See Widget.hpp for the templates of these:
    OptionsBox<T>* setCallback(std::function<void(OptionsBox<T>*)> callback);
    OptionsBox<T>* setCallback(std::function<void()> callback) { return Widget::setCallback(callback); }

protected:
    // Change the currently selected item
    // Note: does not call onUpdate!
    auto update_selection(size_t index);

private:
    void draw(const gfx::RenderContext& ctx) const override;

    // Callbacks
    void onStateChanged(WidgetState state) override;
    void onMouseMoved(float x, float y) override;
    void onMousePressed(float x, float y) override;
    void onMouseReleased(float x, float y) override;
    void onKeyPressed(const sf::Event::KeyEvent& key) override;
    void onKeyReleased(const sf::Event::KeyEvent& key) override;
    void onThemeChanged() override;

    void updateArrowState(ItemBox<Arrow>& arrow, float x, float y);

    struct Item
    {
        std::string label;
        T value;

        Item(const std::string& text, const T& value);
    };

    typedef std::vector<Item> ItemVector;
    ItemVector m_items;
    size_t m_currentIndex;

    // Visual components
    ItemBox<Text> m_box;         // The entire widget (incl. the arrows)
    ItemBox<Arrow> m_arrowLeft;  // Control for "Select Prev."
    ItemBox<Arrow> m_arrowRight; // Control for "Select Next"
};

} // namespace

#include "OptionsBox.inl"

#endif // GUI_OPTIONSBOX_HPP
