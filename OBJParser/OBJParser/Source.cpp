
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif  // _DEBUG

#include "ObjParser.h"
#include "Object.h"
int main(int argc, char* argv[])
{
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	Object o;

	ParseObj("Cube.obj", o._data, o._datap);
	o.Clear();
	ParseObj("Cube.obj", o._data, o._datap);
	o.Clear();
	ParseObj("Cube.obj", o._data, o._datap);
	o.Clear();
	ParseObj("Cube.obj", o._data, o._datap);
	o.Clear();
	ParseObj("Cube.obj", o._data, o._datap);
	o.Clear();
	ParseObj("Cube.obj", o._data, o._datap);
	o.Clear();
	ParseObj("Cube.obj", o._data, o._datap);
	o.Clear();
	ParseObj("Cube.obj", o._data, o._datap);
	o.Clear();
	return 0;
}