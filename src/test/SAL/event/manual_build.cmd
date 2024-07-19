call %~dp0../../../../tooling/_setenv.cmd

g++ -Wall -O1 ^
	-std=c++23 -I../../../../include -I../../../../extern/sfml/mingw/include ^
	-save-temps -fverbose-asm ^
	*.cpp
