#include <string>
#include <cstdint>
#include <iostream>

#include "../../include/sfw/util/utf8.hpp"
using namespace sfw;

//-----------------------------------------
main(int argc, char** argv)
{
	using namespace std;

	string tstr;
	size_t cppos = 0;
	size_t cpcount = string::npos;

	switch (argc) {	
	case 1:	cerr << "Usage: " << argv[0] << " str [cp_pos [cp_count]]\n";
		return 1;
	case 4: cpcount = stoul(argv[3]);
	case 3: cppos = stoul(argv[2]);
	case 2: tstr = argv[1];
		break;
	}

	cout	<< "byte-size of substr("<<tstr<<", "<< cppos << ", "<< cpcount << "): "
		<< utf8_substr_bsize(tstr, cppos, cpcount) <<endl;

	cout	<< "substr("<<tstr<<", "<< cppos << ", "<< cpcount << "): "
		<< utf8_substr(tstr, cppos, cpcount) <<endl;
/*
	cout	<< "code points in \"" << tstr << "\": "
		<< utf8_cpsize(tstr) <<endl;
*/
	return 0;
}
