#ifndef SFW_SHIMS_LANG_HPP
#define SFW_SHIMS_LANG_HPP

#include <string>
#include <cstdint>

namespace sfw {

// Number of code points in UTF-8 string
// Note: will include possibly invalid sequences! (I.e. doesn't validate.)
inline size_t utf8_cpsize(const std::string& str)
{
	int cpsize = 0;
	for (char c : str)
		if ((uint8_t(c) & 0xc0) != 0x80) ++cpsize;
	return cpsize;
}

// Byte-size of the first `cp_limit` code points of a UTF-8 string
// Note: if cp_limit == npos (i.e. not set), the result will include any invalid
//       trailing code units. Use utf8_bsize(str, utf8_cpsize(str)) to trim them.
// (Used e.g. by TextBox to limit string size.)
//!!Rename cp_limit to cp_count to match utf8_substr*(...)!
inline size_t utf8_bsize(const std::string& str, size_t cp_limit = std::string::npos)
{
	size_t bytesize = (cp_limit != std::string::npos) ? 0 : str.size();
	for (size_t cpcount = 0; cpcount < cp_limit && bytesize < str.size(); ++cpcount)
	{
		if (auto c = (std::uint8_t)str[bytesize]; c < 0x80) bytesize += 1;
		else while (c & 0x80) { ++bytesize; c <<= 1; }
	}
	return bytesize;
}

inline size_t utf8_substr_bsize(const std::string& str, size_t cp_pos, size_t cp_count = std::string::npos)
{
	auto bytepos = utf8_bsize(str, cp_pos);
	//!!Optimize: avoid the extra string() either via string_view or via a helper working on C-strings + strlen
	//!!(but then also avoid the extra strlen in the cstr helper)!
	return utf8_bsize(std::string(str.c_str() + bytepos), cp_count);
}

inline std::string utf8_substr(const std::string& str, size_t cp_pos, size_t cp_count = std::string::npos)
{
	auto bytepos = utf8_bsize(str, cp_pos);
	//!!Optimize: avoid the extra string() either via string_view or via a helper working on C-strings + strlen
	//!!(but then also avoid the extra strlen in the cstr helper)!
	return str.substr(bytepos, utf8_bsize(std::string(str.c_str() + bytepos), cp_count));
}

} // namespace

#endif // SFW_SHIMS_LANG_HPP
