#ifndef SFW_GENERIC_HPP
#define SFW_GENERIC_HPP

namespace sfw
{
	struct Param
	{
		enum _symbolic_ : int {
			Unknown = 0,
			UseDefault = -1,
		} value;

		operator int()        { return value; }
	};

	using Param::Unknown,
	      Param::UseDefault;

/*!! This version with the extra ceremony used to be a workaround for a CLANG-14 crash 
     (on the GitHub Ubuntu GHA runner), so leaving it here for a while in case someone
     else also has the misfortune of running into that.

	struct _Param__clang_crash_workaround_
	{
		enum _symbolic_ : int {
			Unknown = 0,
			UseDefault = -1,
		} value;

		operator int()        { return value; }
	};

	using _Param__clang_crash_workaround_::Unknown,
	      _Param__clang_crash_workaround_::UseDefault;

	using Param = _Param__clang_crash_workaround_;
!!*/

} // namespace

#endif // SFW_GENERIC_HPP
