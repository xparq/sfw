@echo off

call %~dP0../../../../tooling/_setenv.cmd

call %PRJ_ROOT_ABS_%tooling/test/build_separately-gcc.cmd ^
	-DSFML_STATIC ^
	%* ^

rem NOT YET:
goto :eof
	-static ^
	%PRJ_ROOT_ABS_%extern\sfml\mingw\lib\libsfml-system-s-d.a ^
	%PRJ_ROOT_ABS_%extern\sfml\mingw\lib\libsfml-window-s-d.a ^
	%PRJ_ROOT_ABS_%extern\sfml\mingw\lib\libsfml-graphics-s-d.a ^
	%PRJ_ROOT_ABS_%lib\mingw\libsfw-s-d.a
