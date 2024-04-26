#ifndef _DOCIU4Y578TB87SX45T687045N78H3565456UHY_
#define _DOCIU4Y578TB87SX45T687045N78H3565456UHY_

#include "sfw/Gfx/Render.hpp"

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>

namespace sfw
{

/**
 * Draw an image, possibly stretched, onto a rectangle
 */
class Wallpaper: public GenericWallpaper, public gfx::Drawable, public sf::Transformable
{
	static constexpr sf::IntRect NullRect = {{0,0}, {0,0}};

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
	Wallpaper(const std::string& filename, const sf::IntRect& r = NullRect);
	Wallpaper(const sf::Image& Wallpaper,  const sf::IntRect& r = NullRect);
	Wallpaper(const sf::Texture& texture,  const sf::IntRect& r = NullRect);

	Wallpaper* setImage(const std::string& filename, const sf::IntRect& r = NullRect);
	Wallpaper* setImage(const sf::Image& image,      const sf::IntRect& r = NullRect);
	Wallpaper* setImage(const sf::Texture& texture,  const sf::IntRect& r = NullRect);

	const sf::Texture& texture() const { return m_texture; }

	Wallpaper*   setSize(sf::Vector2f size);
	sf::Vector2f getSize() const;

	Wallpaper*  setCropRect(const sf::IntRect& r);
	sf::IntRect getCropRect() const;

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
	sf::Texture m_texture;
	sf::Vector2f m_baseSize;
	float m_scalingFactor = 1.f;
	sf::Vertex m_vertices[4];
};

} // namespace sfw

#endif // _DOCIU4Y578TB87SX45T687045N78H3565456UHY_
