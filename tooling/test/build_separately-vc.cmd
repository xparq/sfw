@echo off
call %~dP0..\_setenv.cmd

if     _%1_==__ set "what=*.cpp"
if not _%1_==__ set "what=%1"


@echo on

cl -nologo -EHsc ^
	-W3 ^
	-std:c++latest -I%PRJ_ROOT_ABS_%include -I%PRJ_ROOT_ABS_%extern/sfml/mingw/include -I. ^
	-O1 -Zi ^
	%what%

@echo off
