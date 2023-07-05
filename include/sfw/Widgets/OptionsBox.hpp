#ifndef GUI_OPTIONSBOX_HPP
#define GUI_OPTIONSBOX_HPP

#include "sfw/InputWidget.hpp"
#include "sfw/Gfx/Elements/Text.hpp"
#include "sfw/Gfx/Elements/Arrow.hpp"
#include "sfw/Gfx/Elements/ItemBox.hpp"

#include <string>

namespace sfw
{

/*===========================================================================
  Widget for selecting an item from a list. It can only display the currently
  selected item, so it's essentially a collapsed list box

  The update callback is triggered when the item selection changes.
 ===========================================================================*/

class OptionsBoxBase : public InputWidget<OptionsBoxBase> {}; //! See the comment below...

template <class T>
class OptionsBox: public OptionsBoxBase	//!! Alas, can't do (recursive!...) CRTP for templated descendants, so
                                        //!! `class OptionsBox: public InputWidget<OptionsBox<T>>` won't work.
                                        //!! Need to (1) use an artificial base to inherit the InputWidget stuff,
                                        //!! and (2) do explicit conversions here for integrating it, and may
                                        //!! even need to (3) redefine some of it here anew!... :-/
                                        //!! BUT, THE WORST: (4) It must also be kept in sync with InputWidget manually! :-(
                                        //!! (The compiler would probably catch most of the divergent changes, though.)
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

    //! Kludge API proxy fns. to overcome the InputWidget<...> inheritance limitations (see note above)!
    //! - update(...) just needs to return a downcasted this (for method cahining):
    template <typename V> OptionsBox<T>* update(V value) { return (OptionsBox<T>*)update(value); }
    //! - but the auto-generated OptionsBoxBase::setCallback is totally unusable here, unfortunately. :-/ (No conversion across the callback types!)
    //!   So, we need to define it here from scratch. See the InputWidget class for the "prototype"!
    OptionsBox<T>* setCallback(std::function<void(OptionsBox<T>*)> callback) {
	on(Event::Update, [callback](Event::Handler* w) { callback( (OptionsBox*)w ); });
	return this;
    }


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
