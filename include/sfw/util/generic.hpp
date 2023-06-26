#ifndef SFW_GENERIC_HPP
#define SFW_GENERIC_HPP

namespace sfw
{
	struct _unnamed_enum_Param_thanks_cplusplus_
	{
		enum _symbolic_ : int {
			Unknown = 0,
			UseDefault = -1,
		} value;

		operator int()        { return value; }
	};

	using _unnamed_enum_Param_thanks_cplusplus_::Unknown,
	      _unnamed_enum_Param_thanks_cplusplus_::UseDefault;

 	//!! The extra ceremony is for trying to avoid a CLANG-14 crash! :-o
	using Param = _unnamed_enum_Param_thanks_cplusplus_;

} // namespace

#endif // SFW_GENERIC_HPP
