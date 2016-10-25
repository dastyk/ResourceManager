
#include <iostream>
#include "scanner.h"
#include "parser.tab.h"
#include "ObjParser.h"
#include <fstream>
#pragma warning(disable:4146)

using namespace ObjParser;
using namespace std;
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//https://github.com/ezaquarii/bison-flex-cpp-example

#endif  // _DEBUG

int main(int argc, char **argv) {
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
#endif // _DEBUG
    Interpreter i;
	ifstream file("DUCK.OBJ");
	ArfData::Data data;
	ArfData::DataPointers dp;
	if (file.is_open())
	{
		int res = i.parse(&file, &data, &dp);
		cout << "Parse complete. Result = " << res << endl;
 		cout << i.str() << endl;
		operator delete(data.buffer);

		getchar();

		return res;
	}
	return 0;
}
