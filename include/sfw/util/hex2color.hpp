#ifndef SFW_HEX2COLOR_HPP
#define SFW_HEX2COLOR_HPP

#include <SFML/Graphics/Color.hpp>
#include <string>

sf::Color hex2color(const std::string& hexcolor)
{
    sf::Color color = sf::Color::Black;
    if (hexcolor.size() == 7) // #ffffff
    {
        color.r = (uint8_t)strtoul(hexcolor.substr(1, 2).c_str(), NULL, 16);
        color.g = (uint8_t)strtoul(hexcolor.substr(3, 2).c_str(), NULL, 16);
        color.b = (uint8_t)strtoul(hexcolor.substr(5, 2).c_str(), NULL, 16);
    }
    else if (hexcolor.size() == 4) // #fff
    {
        color.r = (uint8_t)strtoul(hexcolor.substr(1, 1).c_str(), NULL, 16) * 17;
        color.g = (uint8_t)strtoul(hexcolor.substr(2, 1).c_str(), NULL, 16) * 17;
        color.b = (uint8_t)strtoul(hexcolor.substr(3, 1).c_str(), NULL, 16) * 17;
    }
    return color;
}

#endif // SFW_HEX2COLOR_HPP
