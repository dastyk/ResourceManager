

#include "ObjParser.h"
#include <sstream>

using namespace ObjParser;

Interpreter::Interpreter() :
    _scanner(*this),
    _parser(_scanner, *this),
    m_location(0)
{

}

int Interpreter::parse(std::istream *is, ArfData::Data* data, ArfData::DataPointers* datap)
{
	_data = data;
	_datap = datap;	
	Alloc(ALLOC_ALL);
	AddSubMesh("default");
	_scanner.switch_streams(is, NULL);
    m_location = 0;
	int ret = _parser.parse();

	Alloc(ALLOC_SHRINK_TO_FIT);

	return ret;
}

void Interpreter::clear()
{
    m_location = 0;
}

void ObjParser::Interpreter::Alloc(int flag)
{
	ArfData::Data* ndat = new ArfData::Data;
	ArfData::DataPointers* ndatap = new ArfData::DataPointers;

	switch (flag)
	{
	case ALLOC_ALL:
	{
		_data->PosCap = 10000;
		_data->TexCap = 10000;
		_data->NormCap = 10000;
		_data->FaceCap = 10000;
		_data->SubMeshCap = 100;
		_data->NumNorm = 0;
		_data->NumFace = 0;
		_data->NumPos = 0;
		_data->NumSubMesh = 0;
		_data->NumTex = 0;
		_data->allocated = 0;

		break;
	}
	case ALLOC_POSITION:
	{
		_data->PosCap *= 2;
		break;
	}
	case ALLOC_TECCOORD:
	{
		_data->TexCap *= 2;
		break;
	}
	case ALLOC_NORMAL:
	{
		_data->NormCap *= 2;
		break;
	}
	case ALLOC_FACE:
	{
		_data->FaceCap *= 2;
		break;
	}
	case ALLOC_SUB_MESH:
	{
		_data->SubMeshCap *= 2;
		break;
	}
	case ALLOC_SHRINK_TO_FIT:
	{
		_data->PosCap = _data->NumPos;
		_data->TexCap = _data->NumTex;
		_data->NormCap = _data->NumNorm;
		_data->FaceCap = _data->NumFace;
		_data->SubMeshCap = _data->NumSubMesh;
		break;
	}
	default:
		break;
	}
	*ndat = *_data;
	ndat->allocated = 
		_data->PosCap * sizeof(ArfData::Position) +
		_data->TexCap * sizeof(ArfData::TexCoord) +
		_data->NormCap * sizeof(ArfData::Normal) +
		_data->FaceCap * sizeof(ArfData::Face) +
		_data->SubMeshCap * sizeof(ArfData::SubMesh);
	ndatap->buffer = operator new(ndat->allocated);


	ndatap->positions = (ArfData::Position*)ndatap->buffer;
	ndatap->texCoords = (ArfData::TexCoord*)(ndatap->positions + ndat->PosCap);
	ndatap->normals = (ArfData::Normal*)(ndatap->texCoords + ndat->TexCap);
	ndatap->faces = (ArfData::Face*)(ndatap->normals + ndat->NormCap);
	ndatap->subMesh = (ArfData::SubMesh*)(ndatap->faces + ndat->FaceCap);

	memcpy(ndatap->positions, _datap->positions, ndat->NumPos * sizeof(ArfData::Position));
	memcpy(ndatap->texCoords, _datap->texCoords, ndat->NumTex * sizeof(ArfData::TexCoord));
	memcpy(ndatap->normals, _datap->normals, ndat->NumNorm * sizeof(ArfData::Normal));
	memcpy(ndatap->faces, _datap->faces, ndat->NumFace * sizeof(ArfData::Face));
	memcpy(ndatap->subMesh, _datap->subMesh, ndat->NumSubMesh * sizeof(ArfData::SubMesh));

	operator delete(_datap->buffer);
	*_data = *ndat;
	*_datap = *ndatap;
	delete ndat;
	delete ndatap;
}

void ObjParser::Interpreter::AddPosition(const ArfData::Position & pos)
{
	if (_data->NumPos >= _data->PosCap)
	{
		// Allocate more space
		Alloc(ALLOC_POSITION);
	}

	_datap->positions[_data->NumPos] = pos;
	_data->NumPos++;
}

void ObjParser::Interpreter::AddTexCoord(const ArfData::TexCoord & tex)
{
	if (_data->NumTex >= _data->TexCap)
	{
		// Allocate more space
		Alloc(ALLOC_TECCOORD);
	}

	_datap->texCoords[_data->NumTex] = tex;
	_data->NumTex++;
}

void ObjParser::Interpreter::AddNormal(const ArfData::Normal & norm)
{
	if (_data->NumNorm >= _data->NormCap)
	{
		// Allocate more space
		Alloc(ALLOC_NORMAL);
	}

	_datap->normals[_data->NumNorm] = norm;
	_data->NumNorm++;
}

void ObjParser::Interpreter::AddFace(const ArfData::Face & face)
{
	if (_data->NumFace >= _data->FaceCap)
	{
		// Allocate more space->
		Alloc(ALLOC_FACE);
	}

	if (face.indexCount == 4)
	{
		std::vector<std::vector<uint32_t>> face1v;
		for (uint8_t i = 0; i < 3; i++)
		{
			std::vector<uint32_t> in;
			for (uint8_t j = 0; j < face.indices[i].indexCount; j++)
			{
				in.push_back(face.indices[i].index[j]-1);
			}
			face1v.push_back(in);
		}
		_datap->faces[_data->NumFace] = ArfData::Face(face1v);
		if (_data->NumSubMesh)
			_datap->subMesh[_data->NumSubMesh - 1].faceCount++;
		_data->NumFace++;

		uint32_t indices[] = { 2, 3 ,0 };
		face1v.clear();
		for (uint8_t i = 0; i < 3; i++)
		{
			std::vector<uint32_t> in;
			for (uint8_t j = 0; j < face.indices[indices[i]].indexCount; j++)
			{
				in.push_back(face.indices[indices[i]].index[j]-1);
			}
			face1v.push_back(in);
		}
		_datap->faces[_data->NumFace] = ArfData::Face(face1v);
		if (_data->NumSubMesh)
			_datap->subMesh[_data->NumSubMesh - 1].faceCount++;
		_data->NumFace++;
	}
	else if (face.indexCount == 2)
		throw "Fuck yo lines bitch";
	else
	{


		_datap->faces[_data->NumFace] = face;
		if (_data->NumSubMesh)
			_datap->subMesh[_data->NumSubMesh - 1].faceCount++;
		_data->NumFace++;
	}
}

void ObjParser::Interpreter::AddSubMesh(const string & name)
{
	if (_data->NumSubMesh >= _data->SubMeshCap)
	{
		// Allocate more space->
		Alloc(ALLOC_SUB_MESH);
	}
	int size = min((int)name.size(), SUBMESH_NAME_MAX_LENGHT - 1);
	if (_data->NumSubMesh == 1)
	{
		if (std::string(_datap->subMesh[0].name) == "default")
		{
			
			memcpy(_datap->subMesh[0].name, name.c_str(), size);
			_datap->subMesh[0].name[size] = '\0';
			return;
		}
	}
	memcpy(_datap->subMesh[_data->NumSubMesh].name, name.c_str(), size);
	_datap->subMesh[_data->NumSubMesh].name[size] = '\0';
	_datap->subMesh[_data->NumSubMesh].faceStart = _data->NumFace;
	_datap->subMesh[_data->NumSubMesh].faceCount = 0;
	_data->NumSubMesh++;
}


std::string Interpreter::str() const 
{
    std::stringstream s;
	s << "Positions: " << endl;
	for (uint64_t i = 0; i < _data->NumPos; i++)
	{
		s << "\t" << _datap->positions[i].x << " " << _datap->positions[i].y << " " << _datap->positions[i].z/* << " " << _datap.positions[i].w*/ << endl;
	}
	s << "Texcoords: " << endl;
	for (uint64_t i = 0; i < _data->NumTex; i++)
	{
		s << "\t" << _datap->texCoords[i].u << " " << _datap->texCoords[i].v/* << " " << _datap->texCoords[i].w << " "*/ << endl;
	}
	s << "Normals: " << endl;
	for (uint64_t i = 0; i < _data->NumNorm; i++)
	{
		s << "\t" << _datap->normals[i].x << " " << _datap->normals[i].y << " " << _datap->normals[i].z << endl;
	}
	if (_data->NumSubMesh)
	{
		s << "SubMesh: " << endl;
		for (uint32_t n = 0; n < _data->NumSubMesh; n++)
		{
			s << "\t" << _datap->subMesh[n].name << endl;
			for (uint64_t i = _datap->subMesh[n].faceStart; i < _datap->subMesh[n].faceCount + _datap->subMesh[n].faceStart; i++)
			{
				s << "\t\t";
				for (uint8_t j = 0; j < _datap->faces[i].indexCount; j++)
				{
					for (uint8_t k = 0; k < _datap->faces[i].indices[j].indexCount - 1; k++)
					{
						s << _datap->faces[i].indices[j].index[k] << "/";
					}
					if (_datap->faces[i].indices[j].indexCount > 0)
						s << _datap->faces[i].indices[j].index[_datap->faces[i].indices[j].indexCount - 1] << " ";
				}
				s << endl;
			}
		}
	}
	else
	{
		s << "Faces: " << endl;
		for (uint64_t i = 0; i < _data->NumFace; i++)
		{
			s << "\t";
			for (uint8_t j = 0; j < _datap->faces[i].indexCount; j++)
			{
				for (uint8_t k = 0; k < _datap->faces[i].indices[j].indexCount - 1; k++)
				{
					s << _datap->faces[i].indices[j].index[k] << "/";
				}
				if (_datap->faces[i].indices[j].indexCount > 0)
					s << _datap->faces[i].indices[j].index[_datap->faces[i].indices[j].indexCount - 1] << " ";
			}
			s << endl;
		}
	}
    return s.str();
}

void Interpreter::increaseLocation(unsigned int loc) 
{
    m_location += loc;
  //  cout << "increaseLocation(): " << loc << ", total = " << m_location << endl;
}

unsigned int Interpreter::location() const 
{
    return m_location;
}
