::
:: Run tests/examples built with either the DLL version of SFML or the debug
:: version of the MSVCRT redist. DLLs (which aren't on the PATH in my env.)
::

@echo off

if "%1" == "" (
	echo Usage: run some/file.exe [params...]
	echo.
	echo ^(It just sets the PATH for finding the DLLs.^)
	exit
)

::!! Doesn't even try to detect and work also in a mingw env., sorry...:
PATH=%PATH%;extern\sfml\msvc\bin;tooling\Microsoft.VC143.DebugCRT.tmp

%*
