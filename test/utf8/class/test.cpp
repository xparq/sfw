#include "utf8string.hpp"

//---------------------------------------------------------------
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
	if (argc <= 1) {
		cerr
			<< "Usage: " << argv[0] << " length str\n"
			<< "       " << argv[0] << " bytes  str [cp_count]\n"
			<< "       " << argv[0] << " substr str [cp_pos [cp_count]]\n"
		;
		return 1;
	}

	utf8string method = argv[1];
	utf8string tstr = argc > 2 ? argv[2] : "";

	if (method == "length") {
		cout	<< "codepoints in \"" << tstr << "\": "
			<< tstr.u8length() <<endl;

	} else if (method == "bsize") {
		size_t cplimit = argc > 3 ? stoul(argv[3]) : string::npos;
		if (cplimit != string::npos) {
			cout	<< "byte-size of the first " << cplimit << " codepoints of \"" << tstr << "\": "
				<< tstr.u8bytes(cplimit) <<endl;
		} else {
			cout	<< "byte-size of \"" << tstr << "\": "
				<< tstr.u8bytes() <<endl;
		}

	} else if (method == "substr") {
		size_t cppos   = argc > 3 ? stoul(argv[3]) : string::npos;
		size_t cpcount = argc > 4 ? stoul(argv[4]) : string::npos;

		cout	<< "substr("<<tstr<<", "               << cppos << ", "<< (cpcount != string::npos ? to_string(cpcount) : "") << "): "
			<< tstr.u8substr(cppos, cpcount) <<endl;

		cout	<< "- byte-size of substr("<<tstr<<", "<< cppos << ", "<< (cpcount != string::npos ? to_string(cpcount) : "")  << "): "
			<< tstr.u8substr_bytes(cppos, cpcount) <<endl;

	} else {
cerr <<"- Unknown method '" << method << "'!\n";
	}

/*
	{ utf8string s; }

	{ utf8string str = "áéíÓŐŰ";
		cout << str << endl;
		cout << "length: " << str.length() << endl;
	}

	{ utf8string str = u8"áéíÓŐŰ";
		cout << str << endl;
		cout << "length: " << str.length() << endl;
	}
*/
}
