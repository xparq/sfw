#ifndef _DOCIU4Y578TB87SX45T687045N78H3565456UHY_
#define _DOCIU4Y578TB87SX45T687045N78H3565456UHY_

#include "sfw/gfx/Render.hpp"
#include "sfw/math/Vector.hpp"
#include "sfw/geometry/Rectangle.hpp"
#include "sfw/gfx/element/Texture.hpp"

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>

#include <string_view>

namespace sfw
{

/**
 * Draw an image, possibly stretched, onto a rectangle
 */
class Wallpaper: public GenericWallpaper, public gfx::Drawable, public sf::Transformable
{
	static constexpr geometry::iRect NullRect{};

public:
	struct Cfg
	{
		std::string filename;
		Placement placement;
		sf::Color tint;

		Cfg(const std::string& f = "", Wallpaper::Placement p = Center, sf::Color c = sf::Color::White)
		:
			filename(f),
			placement(p),
			tint(c)
		{}
	};


	Wallpaper();
	Wallpaper(std::string_view filename,   const geometry::iRect& r = NullRect);
	Wallpaper(const sf::Image& image,      const geometry::iRect& r = NullRect);
	Wallpaper(const sfw::Texture& texture, const geometry::iRect& r = NullRect);

	Wallpaper* setImage(std::string_view filename,   const geometry::iRect& r = NullRect);
	Wallpaper* setImage(const sf::Image& image,      const geometry::iRect& r = NullRect);
	Wallpaper* setImage(const sfw::Texture& texture, const geometry::iRect& r = NullRect);

	const sfw::Texture& texture() const { return m_texture; }

	Wallpaper* setSize(iVec2 size);
	iVec2      getSize() const;

	Wallpaper*      setCropRect(const geometry::iRect& r);
	geometry::iRect getCropRect() const;

	// Scaling based on the original image size

	Wallpaper* scale(float factor);
	float      scale() const { return m_scalingFactor; }
	// Relative scaling based on the current size
	Wallpaper* rescale(float factor);

	Wallpaper* setColor(const sf::Color& color);
	sf::Color  getColor() const;

public: //! <- NOT private, because draw() may be accessed directly (statically),
        //!    rather than just polymorphically (dynamically) via a pointer!
	void draw(const gfx::RenderContext& ctx) const override;

private:
	sfw::/*!!gfx::!!*/Texture m_texture;
	iVec2 m_baseSize;
	float m_scalingFactor = 1.f;
	sf::Vertex m_vertices[4];
};

} // namespace sfw

#endif // _DOCIU4Y578TB87SX45T687045N78H3565456UHY_
