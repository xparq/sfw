#ifndef SFW_WALLPAPER_SFML_HPP
#define SFW_WALLPAPER_SFML_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>

namespace sfw
{

/**
 * Draw an image, possibly stretched, onto a rectangle
 */
class Wallpaper: public GenericWallpaper, public sf::Drawable, public sf::Transformable
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

private:
//    void draw(const gfx::RenderContext& ctx) const override;
	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

	sf::Texture m_texture;
	sf::Vector2f m_baseSize;
	float m_scalingFactor = 1.f;
	sf::Vertex m_vertices[4];
};

} // namespace

#endif // SFW_WALLPAPER_SFML_HPP
