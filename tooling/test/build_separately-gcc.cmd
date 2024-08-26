@echo off
call %~dP0..\_setenv.cmd

if     _%1_==__ set "what=*.cpp"
if not _%1_==__ set "what=%*"


@echo on

g++ -Wall -Wno-unused-but-set-variable ^
	-std=c++23 -I%PRJ_ROOT_ABS_%include -I%PRJ_ROOT_ABS_%extern/sfml/mingw/include -I. ^
	-O1 -g ^
	-save-temps -fverbose-asm ^
	%what%

@echo off
