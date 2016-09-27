#ifndef __cplusplus
typedef nullptr NULL;
#endif
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
