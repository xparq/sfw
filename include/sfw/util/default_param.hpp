#ifndef _FOIJUERHTNJV8N7VYHWMR6807CY4VH87WHX978CVHNBUIF_
#define _FOIJUERHTNJV8N7VYHWMR6807CY4VH87WHX978CVHNBUIF_


//!! Just another half-baked experimental idea trying to fight C++ again...


namespace sfw //!!::... (sg. lang?)
{
	struct Param //!! RENAME!
	{
		enum _symbolic_ : int {
			Unknown = 0,
			UseDefault = -1,
		} value;

		constexpr operator int() const { return value; }
		constexpr operator int()       { return value; }
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

} // namespace sfw //!!::... (sg. lang?)


#endif // _FOIJUERHTNJV8N7VYHWMR6807CY4VH87WHX978CVHNBUIF_
