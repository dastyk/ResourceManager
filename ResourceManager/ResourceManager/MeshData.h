#pragma once

#include <stdint.h>
namespace MeshData
{
	struct Position
	{
		float x, y, z;
	};

	struct TexCoord
	{
		float u, v;
	};

	struct Normal
	{
		float x, y, z;
	};

	struct Vertex
	{
		Position pos;
		Normal norm;
		TexCoord tex;
	};

	struct MeshData
	{
		uint32_t NumVertices;
		Vertex* vertices;
		uint32_t IndexCount;
		uint32_t* Indices;
	};
}