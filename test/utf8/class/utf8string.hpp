#ifndef SFW_SHIMS_LANG_HPP
#define SFW_SHIMS_LANG_HPP

#include <string>
#include <cstdint>

class utf8string : public std::string
{
public:
	using std::string::string;

	//!!Somehow, somewhere: static_assert(char8_t is compatible with chartype)
	utf8string(const char8_t* u8literal)
		: std::string((const char*)u8literal) {}

	utf8string(const std::string& str)
		: std::string(str) {}

	// "Length" is somewhat incorrect defined as the number of code points for now...
	// Note: this implementation doesn't check & ignore invalid sequences!
	size_t u8length() const
	{
		int cp = 0;
		for (char c : *this)
			if ((uint8_t(c) & 0xc0) != 0x80) ++cp;
		return cp;
	}

	// Byte-size of the first `cp_limit` code points
	// Note: if cp_limit == npos (i.e. not set), the result will include any invalid
	//       trailing code units. Use u8bytes(u8length()) to trim them.
	size_t u8bytes(size_t cp_limit = std::string::npos)
	{
		size_t bytesize = (cp_limit != std::string::npos) ? 0 : std::string::size();
		for (size_t cp = 0; cp < cp_limit && bytesize < std::string::size(); ++cp)
		{
			if (std::uint8_t c = (*this)[bytesize]; c < 0x80) bytesize += 1;
			else while (c & 0x80) { ++bytesize; c <<= 1; }
		}
		return bytesize;
	}

	size_t u8substr_bytes(size_t cp_pos, size_t cp_count = std::string::npos)
	{
		auto bytepos = u8bytes(cp_pos);
		//!!Optimize: avoid the extra string() either via string_view or via a helper working on C-strings + strlen
		//!!(but then also avoid the extra strlen in the cstr helper)!
		utf8string tmp(c_str() + bytepos);
		return tmp.u8bytes(cp_count);
	}

	utf8string u8substr(size_t cp_pos, size_t cp_count = std::string::npos)
	{
		auto bytepos = u8bytes(cp_pos);
		//!!Optimize: avoid the extra string() either via string_view or via a helper working on C-strings + strlen
		//!!(but then also avoid the extra strlen in the cstr helper)!
		utf8string tmp(c_str() + bytepos);
		//!!Optimize: Also avoid this other extra utf8string() copy!...
		return utf8string(std::string::substr(bytepos, tmp.u8bytes(cp_count)));
	}

}; // class

#endif // SFW_SHIMS_LANG_HPP
