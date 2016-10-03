#include <string>
#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory.h>
#include <fstream>
#include "../ArfData.h"
using namespace std;
using namespace ArfData;

#define ALLOC_ALL 0
#define ALLOC_POSITION 1
#define ALLOC_TECCOORD 2
#define ALLOC_NORMAL 3
#define ALLOC_FACE 4
#define ALLOC_SUB_MESH 5
#define ALLOC_SHRINK_TO_FIT 6

#define GENERATE_ARF_FLAG_NONE 0
#define GENERATE_ARF_FLAG_SHRINK_TO_FIT 1
class Object
{
private:
	Data _data;
	DataPointers _datap;

	void Alloc(int flag);
public:
	Object();
	Object(ifstream & file);
	~Object();
	Object& operator=(Object& other);
	void AddPosition(const Position& pos);
	void AddTexCoord(const TexCoord& tex);
	void AddNormal(const Normal& norm);
	void AddFace(const Face& face);
	void AddSubMesh(const string& name);
	void ShrinkToFit();
	void Triangulate();
	void Print();
	void Clear();
	void GenerateArf(ofstream& of, int flag = GENERATE_ARF_FLAG_SHRINK_TO_FIT);
};
