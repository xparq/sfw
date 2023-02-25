#ifndef GUI_BOX_HPP
#define GUI_BOX_HPP

#include "../Widget.hpp"

namespace gui
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

    /**
     * Get box dimensions
     */
    sf::Vector2f getSize() const;

    void press();

    void release();

    /**
     * @return true if point is inside the box limits
     */
    bool containsPoint(float x, float y) const;

    void applyState(State state);

    template <class T>
    void centerItem(T& item)
    {
        sf::Vector2f size = getSize();
        sf::Vector2f itemSize = item.getSize();
        // Center item
        item.setPosition(
            {getPosition().x + (size.x - itemSize.x) / 2, getPosition().y + (size.y - itemSize.y) / 2}
        );
    }

    void centerTextHorizontally(sf::Text& item);

    void centerTextVertically(sf::Text& item);

protected:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

    virtual void onPress() {};
    virtual void onRelease() {};

    Type m_type;

private:

    // The box is a 9-cell rect. of 3 lines (strips) with 9 vertices each:
    // 8 to draw the cells, plus 1 more for "carriage return", driving the
    // TriangleStrip back to the left of the next line, via 2 degenerate
    // triangles (6-7-8, 7-8-9):
    //
    // 0 2 4 6                                  (6)
    // |/|/|/|  ->  then 8 := 7  ->              |
    // 1 3 5 7                       (1)-(3)-(5)-8
    //
    // Now 9 goes back to 1, starting the 2nd line, and the process repeats.

    static constexpr size_t VERTICES_PER_STRIP = 9;
    static constexpr size_t VERTEX_COUNT = 3 * VERTICES_PER_STRIP;

    enum Strip
    {
        TOP_STRIP = 0,
        MIDDLE_STRIP,
        BOTTOM_STRIP,
    };

    enum Vertex
    {
        TOP_LEFT = 0,
        BOTTOM_RIGHT = VERTICES_PER_STRIP * 3 - 1,
    };

    /**
     * Set the geometry for one line of the 9 slices
     */
    void setStripGeometry(Strip strip, float x0, float x2, float x4, float x6, float top, float bottom);

    /**
     * Set the texture coords for one line of the 9 slices
     */
    void setStripTextureCoords(Strip strip, float txleft, float txtop, float txwidth, float txheight);


    State m_state;

    sf::Vertex m_vertices[VERTEX_COUNT];
};

}

#endif // GUI_BOX_HPP
