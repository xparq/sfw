@echo off

if "%1" == "" (
	echo Usage: run name_of_exe_built_with_dlls [params...]
	echo.
	echo ^(It just sets the PATH for the exe to find its linked DLLs.^)
	exit
)

PATH=%PATH%;extern\sfml\msvc\bin;tooling\Microsoft.VC143.DebugCRT.tmp

%*
