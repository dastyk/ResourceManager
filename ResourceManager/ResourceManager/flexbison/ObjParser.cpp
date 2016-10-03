#include <fstream>
#include <iostream>

#include "parser.tab.h"
#include "ObjParser.h"

extern Object o;
extern FILE * yyin;
void yy::parser::error(std::string const&err)
{
	//std::cout << "It's one of the bad ones... " << err << std::endl;
	std::cout << err << std::endl;
}

void ParseObj(char* rawData, MeshData::MeshData& mdata)
{
	o.Clear();
	fopen_s(&yyin, "temp", "r");

	setbuf(yyin, rawData);

	yy::parser parser;
	if (parser.parse())
		throw std::runtime_error("Could not parse obj");

	// Setup pointers
	ArfData::Data& data = o.GetData();
	ArfData::DataPointers& datap = o.GetDataP();

	// Interleave data
	mdata.NumVertices = data.NumFace * 3;
	mdata.vertices = new MeshData::Vertex[mdata.NumVertices];
	uint32_t index = 0;
	for (uint32_t i = 0; i < data.NumSubMesh; i++)
	{
		for (uint32_t j = datap.subMesh[i].faceStart; j < datap.subMesh[i].faceCount; j++)
		{
			auto& face = datap.faces[j];

			for (uint8_t r = 0; r < face.indexCount; r++)
			{

				auto& ind = face.indices[r];
				// Positions
				memcpy(&mdata.vertices[index].pos, &datap.positions[ind.index[0]], sizeof(MeshData::Position));
				// Normals
				memcpy(&mdata.vertices[index].norm, &datap.normals[ind.index[2]], sizeof(MeshData::Normal));
				// TexCoords
				memcpy(&mdata.vertices[index].tex, &datap.texCoords[ind.index[1]], sizeof(MeshData::TexCoord));

				index++;
			}

		}
	}

	mdata.IndexCount = data.NumFace * 3;
	mdata.Indices = new uint32_t[mdata.IndexCount];
	for (uint32_t i = 0; i < mdata.IndexCount; i++)
	{
		mdata.Indices[i] = i;
	}


	fclose(yyin);
}


//int main(int argc, char* argv[])
//{
//	argc = 2;
//	argv[1] = "test.obj";
//	if (argc > 1)
//	{
//		yyin = nullptr;
//		cout << "Args > 1 File: " << argv[1] << endl;
//		if (argc > 2)
//			cout << "Args > 2 Output: " << argv[2] << endl;
//		else
//			cout << "Output: out.arf" << endl;
//		fopen_s(&yyin,argv[1], "r");
//		if (!yyin)
//		{
//			cout << "Failed to open file." << endl;
//			return -1;
//		}
//
//		cout << "Compiling..." << endl << endl;
//		yy::parser parser;
//
//
//		if (!parser.parse())
//		{
//			cout << "Compiled" << endl;
//			//o.Print();
//			ofstream file;
//			if (argc > 2)
//				file.open(argv[2], ios::binary | ios::trunc);
//			else
//				file.open("out.arf", ios::binary | ios::trunc);
//			o.GenerateArf(file);
//			file.close();
//		}
//	}
//	return 0;
//}