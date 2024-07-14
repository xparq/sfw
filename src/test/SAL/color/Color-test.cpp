#include "SAL/gfx/Color.hpp"

#include <iostream>
#include <fstream> // just for updating the run-count file :)
#include <type_traits> // is_same

using namespace std;
using namespace SAL;
//namespace geo = SAL::geometry;
using namespace SAL::gfx;


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
int main()
{
ofstream("run_count.tmp", std::ios_base::app) << "x";
}
