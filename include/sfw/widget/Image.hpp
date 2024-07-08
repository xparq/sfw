#ifndef _DCMUSDTJYHNYU340E79TNY78RYTM7G98DFVIUGHJ_
#define _DCMUSDTJYHNYU340E79TNY78RYTM7G98DFVIUGHJ_


#include "sfw/Widget.hpp"
#include "SAL/gfx/element/Texture.hpp"
#include "SAL/gfx/element/TexturedVertex2.hpp"

//!!#include "SAL/geometry/Rectangle.hpp"
#include "sfw/geometry/Rectangle.hpp"

#include <string_view>

#include <SFML/Graphics/Color.hpp>


namespace sfw
{

/**
 * Widget for displaying (part of) a picture
 * Passive widget: can't react to inputs or trigger events
 */
class Image: public Widget
{
    static constexpr geometry::iRect NullRect{};

public:
    Image();
    Image(std::string_view filename,  const geometry::iRect& r = NullRect);
    Image(const sf::Image& image,     const geometry::iRect& r = NullRect);
    Image(const SAL::gfx::Texture& texture, const geometry::iRect& r = NullRect);

    Image* setTexture(std::string_view filename,  const geometry::iRect& r = NullRect);
    Image* setTexture(const sf::Image& image,     const geometry::iRect& r = NullRect);
    Image* setTexture(const SAL::gfx::Texture& texture, const geometry::iRect& r = NullRect);

    const SAL::gfx::Texture& texture() const { return m_texture; }

    Image* setCropRect(const geometry::iRect& r);
    geometry::iRect cropRect() const;

    Image* scale(float factor);
    float  scale() const { return m_scalingFactor; }
    // Relative scaling based on the current size, not the original
    Image* rescale(float factor);

    Image* setColor(const sf::Color& color);

private:
    void draw(const gfx::RenderContext& ctx) const override;
    void onResized() override;

    SAL::gfx::Texture m_texture;
    fVec2 m_baseSize;
    float m_scalingFactor = 1.f;
    SAL::gfx::TexturedVertex2 m_vertices[4];
};

} // namespace sfw


#endif // _DCMUSDTJYHNYU340E79TNY78RYTM7G98DFVIUGHJ_
