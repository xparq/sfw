#ifndef _LASKDRUHTNIUSCE5H7Y7N4W0987YW4U58MSDHFMYKJHN_
#define _LASKDRUHTNIUSCE5H7Y7N4W0987YW4U58MSDHFMYKJHN_


#include <SFML/System/String.hpp>
#include <string>
#include <string_view>


namespace SAL//!!::adapter::SFML
{

//!! This would require an iterator to `end` -- but if they mean an utf-8 iterator,
//!! rather than byte (`char`), then I can't give it to them this way, here, now...
//!! (The API ref. doesn't say what type it should be, it's just a template param...)
//!!    m_box.item().setString(sf::String::fromUtf8(text));
//!!
//!! So, let's just stick to the "utf-8 everywhere" principle, and do:
//!!    m_box.item().setString(text);
//!!
//!! Except... That didn't seem to work with SFML... :-/ Same broken decoding. WTH?!
//!! OK, *sigh*, still trying the original choice then...
//!! But that would probably crash for char-length >= utf8-length! :(
inline sf::String stdstring_to_SFMLString(const std::string& stdstr)
{
	return sf::String::fromUtf8(stdstr.c_str(), stdstr.c_str() + stdstr.length());
	//! Wow, OK, it didn't crash, and even seems to work fine! :-o
	//! So they do need plain "byte iterators" for this then?! Phew...
}	//! (Can't quite make sense of that, but whatever -- let's celebrate! :) )

//!! Note: string_view IS NOT NULL-TERMINATED! :-o
inline sf::String stdstringview_to_SFMLString(std::string_view sv)
{
	return sf::String::fromUtf8(sv.begin(), sv.end());
}


//!!See above about not trying to be too utf8-smart for nothing...
inline std::string SFMLString_to_stdstring(const sf::String& sfstr)
{
	// Need recasting (-> recopy... :-/ ) from SFML's string<uint8_t> to string<char>:
	return (const char*)sfstr.toUtf8().c_str();
}


} // namespace SAL//!!::adapter::SFML


#endif // _LASKDRUHTNIUSCE5H7Y7N4W0987YW4U58MSDHFMYKJHN_
