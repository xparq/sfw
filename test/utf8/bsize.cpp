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
	size_t cplimit = 1;

	switch (argc) {	
	case 1:	cerr << "Usage: " << argv[0] << " str [cp_count]\n";
		return 1;
	case 3: cplimit = stoul(argv[2]);
	case 2: tstr = argv[1];
		break;
	}

	cout	<< "byte-size of the first " << cplimit << " codepoints of \"" << tstr << "\": "
		<< utf8_bsize(tstr, cplimit) <<endl;

	cout	<< "byte-size of the whole \"" << tstr << "\": "
		<< utf8_bsize(tstr) <<endl;

	cout	<< "code points in \"" << tstr << "\": "
		<< utf8_cpsize(tstr) <<endl;

	return 0;
}
