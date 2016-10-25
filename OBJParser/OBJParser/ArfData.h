#ifndef __cplusplus
typedef nullptr NULL;
#endif


#ifndef _ARF_DATA_H_
#define _ARF_DATA_H_
#include <stdint.h>
#include <vector>
#include <stdint.h>

namespace ArfData
{
	struct Position
	{
		float x, y, z;
		Position(float x, float y, float z) : x(x), y(y), z(z)
		{}
	};

	struct TexCoord
	{
		float u, v;
		TexCoord(float u, float v) : u(u), v(v)
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
		Indices indices[3]; // Array of size 4.
		Face(std::vector<std::vector<uint64_t>>& face) : indexCount(0)
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
		char name[20];
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
}

#endif