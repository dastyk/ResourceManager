

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
	_scanner.switch_streams(is, NULL);
    m_location = 0;
    return _parser.parse();
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
		_data->buffer = operator new(sizeof(ArfData::Data));
		memset(_data->buffer, 0, sizeof(ArfData::Data));

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
	ndat->allocated = sizeof(ArfData::Data) +
		_data->PosCap * sizeof(ArfData::Position) +
		_data->TexCap * sizeof(ArfData::TexCoord) +
		_data->NormCap * sizeof(ArfData::Normal) +
		_data->FaceCap * sizeof(ArfData::Face) +
		_data->SubMeshCap * sizeof(ArfData::SubMesh);
	ndat->buffer = operator new(ndat->allocated);

	memcpy(ndat->buffer, _data->buffer, sizeof(ArfData::Data));

	_data->PosStart = sizeof(ArfData::Data);
	ndatap->positions = (ArfData::Position*)((size_t)ndat->buffer + ndat->PosStart);

	ndat->TexStart = ndat->PosStart + ndat->PosCap * sizeof(ArfData::Position);
	ndatap->texCoords = (ArfData::TexCoord*)((size_t)ndat->buffer + ndat->TexStart);

	ndat->NormStart = ndat->TexStart + ndat->TexCap * sizeof(ArfData::TexCoord);
	ndatap->normals = (ArfData::Normal*)((size_t)ndat->buffer + ndat->NormStart);

	ndat->FaceStart = ndat->NormStart + ndat->NormCap * sizeof(ArfData::Normal);
	ndatap->faces = (ArfData::Face*)((size_t)ndat->buffer + ndat->FaceStart);

	ndat->SubMeshStart = ndat->FaceStart + ndat->FaceCap * sizeof(ArfData::Face);
	ndatap->subMesh = (ArfData::SubMesh*)((size_t)ndat->buffer + ndat->SubMeshStart);

	memcpy(ndatap->positions, _datap->positions, ndat->NumPos * sizeof(ArfData::Position));
	memcpy(ndatap->texCoords, _datap->texCoords, ndat->NumTex * sizeof(ArfData::TexCoord));
	memcpy(ndatap->normals, _datap->normals, ndat->NumNorm * sizeof(ArfData::Normal));
	memcpy(ndatap->faces, _datap->faces, ndat->NumFace * sizeof(ArfData::Face));
	memcpy(ndatap->subMesh, _datap->subMesh, ndat->NumSubMesh * sizeof(ArfData::SubMesh));

	operator delete(_data->buffer);
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
	if (_data->NumPos >= _data->PosCap)
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

	_datap->faces[_data->NumFace] = face;
	if (_data->NumSubMesh)
		_datap->subMesh[_data->NumSubMesh - 1].faceCount++;
	_data->NumFace++;
}

void ObjParser::Interpreter::AddSubMesh(const string & name)
{
	if (_data->NumSubMesh >= _data->SubMeshCap)
	{
		// Allocate more space->
		Alloc(ALLOC_SUB_MESH);
	}
	if (_data->NumSubMesh == 1)
	{
		if (std::string(_datap->subMesh[0].name) == "default")
		{
			memcpy(_datap->subMesh[0].name, name.c_str(), name.size() + 1);
			return;
		}
	}
	memcpy(_datap->subMesh[_data->NumSubMesh].name, name.c_str(), name.size() + 1);
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
