#ifndef GUI_BOX_SFML_HPP
#define GUI_BOX_SFML_HPP

#include "sfw/WidgetState.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>

#include <cmath>
#include <optional>

namespace sfw
{

/**
 * Utility class used by widgets for holding visual components
 */
class Box: public sf::Drawable
{
public:
    enum Type
    {
        Input,
        Click
    };

    Box(Type type = Click);

    /**
     * Get box position
     */
    const sf::Vector2f& getPosition() const;

    void setPosition(float x, float y);

    /**
     * Set the box dimensions
     */
    void setSize(float width, float height);
    void setSize(const sf::Vector2f& size) { return setSize(size.x, size.y); }

    /**
     * Get box dimensions
     */
    sf::Vector2f getSize() const;

    /**
     * Fill the box with this color, overriding the texture
     * (The borders will be left unchanged.)
     */
    void setFillColor(sf::Color color);

    void press();
    void release();

    /**
     * @return true if a point is inside the box (including its borders)
     */
    bool contains(float x, float y) const;

    void applyState(WidgetState state);

    template <class T>
    void centerItem(T& item)
    {
        sf::Vector2f size = getSize();
        sf::Vector2f itemSize = item.getSize();
        // Center item
        item.setPosition(
            {roundf(getPosition().x + (size.x - itemSize.x) / 2),
             roundf(getPosition().y + (size.y - itemSize.y) / 2)}
        );
    }

    void centerTextHorizontally(sf::Text& text);
    void centerVerticalTextVertically(sf::Text& text);

protected:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    virtual void onPress() {};
    virtual void onRelease() {};

    Type m_type;

private:

    // A Box is drawn using a triangle strip (sf::TriangleStrip) that
    // consists of 3 isomorphic rows (segments) of 9 vertices each:
    // 8 to draw the (textured) rectangles, + 1 more for "carriage return",
    // driving the triangle strip back to the left of the next segment,
    // via 2 degenerate triangles (6-7-8, 7-8-9):
    //
    // 0 2 4 6                                  (6)
    // |/|/|/|  ->  then 8 := 7  ->              |
    // 1 3 5 7                       (1)-(3)-(5)-8
    //
    // Now 9 goes back to 1, and the process repeats with the 2nd row; etc.
    // Note that the next segment will start by repeating this last vertex,
    // resulting in yet another degenerate triangle.

    static constexpr size_t VERTICES_PER_SEGMENT = 9;
    static constexpr size_t VERTEX_COUNT = 3 * VERTICES_PER_SEGMENT;

    enum StripSegment
    {
        TOP_SEGMENT,
        MIDDLE_SEGMENT,
        BOTTOM_SEGMENT,
    };

    enum Vertex
    {
        TOP_LEFT,
        BOTTOM_RIGHT = VERTICES_PER_SEGMENT * 3 - 1,
    };

    /**
     * Set the geometry for one line of the 9 slices
     */
    void setSegmentGeometry(StripSegment n, float x0, float x2, float x4, float x6, float top, float bottom);

    /**
     * Set the texture coords for one line of the 9 slices
     */
    void setSegmentTextureCoords(StripSegment n, float txleft, float txtop, float txwidth, float txheight);


    WidgetState m_state;
    sf::Vertex m_vertices[VERTEX_COUNT];
    std::optional<sf::Color> m_fillColor;
};

} // namespace

#endif // GUI_BOX_SFML_HPP
