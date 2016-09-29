#include <string>
#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory.h>
#include <fstream>
using namespace std;

struct Position
{
	float x, y, z;//, w;
	Position(float x, float y, float z) : x(x), y(y), z(z)//, float w) : x(x), y(y), z(z), w(w)
	{}
};

struct TexCoord
{
	float u, v;//, w;
	TexCoord(float u, float v) : u(u), v(v)//, float w) : u(u), v(v), w(w)
	{}
};

struct Normal
{
	float x, y, z;
	Normal(float x, float y, float z) : x(x), y(y), z(z)
	{}
};

struct Indices
{
	uint8_t indexCount;
	uint64_t index[3];// array of size 4.
};

struct Face
{
	uint8_t indexCount;
	Indices indices[4]; // Array of size 4.
	Face(vector<vector<uint64_t>>& face) : indexCount(0)
	{
		indexCount = face.size();

		for (uint8_t i = 0; i < face.size(); i++)
		{
			indices[i].indexCount = face[i].size();
			for (uint8_t j = 0; j < face[i].size(); j++)
			{
				indices[i].index[j] = face[i][j];
			}
		}
	}

};

struct SubMesh
{
	char name[100];
	uint64_t faceStart;
	uint64_t faceCount;
};

struct Data
{
	void* buffer = nullptr;
	size_t allocated = 0;

	size_t PosStart = 0;
	uint64_t NumPos = 0;
	uint64_t PosCap = 0;

	size_t TexStart = 0;
	uint64_t NumTex = 0;
	uint64_t TexCap = 0;

	size_t NormStart = 0;
	uint64_t NumNorm = 0;
	uint64_t NormCap = 0;

	size_t FaceStart = 0;
	uint64_t NumFace = 0;
	uint64_t FaceCap = 0;

	size_t SubMeshStart = 0;
	uint32_t NumSubMesh = 0;
	uint32_t SubMeshCap = 0;
};

struct DataPointers
{
	Position* positions;
	TexCoord* texCoords;
	Normal* normals;
	Face* faces;
	SubMesh* subMesh;
};
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
	void GenerateArf(ofstream& of, int flag = GENERATE_ARF_FLAG_SHRINK_TO_FIT);
};
