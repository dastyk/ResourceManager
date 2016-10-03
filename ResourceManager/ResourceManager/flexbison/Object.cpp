#include "Object.h"
using namespace std;

void Object::Alloc(int flag)
{
	Data ndat;
	DataPointers ndatap;

	switch (flag)
	{
	case ALLOC_ALL:
	{
		_data.buffer = operator new(sizeof(Data));
		memset(_data.buffer, 0, sizeof(Data));

		_data.PosCap = 10000;
		_data.TexCap = 10000;
		_data.NormCap = 10000;
		_data.FaceCap = 10000;
		_data.SubMeshCap = 100;
		break;
	}
	case ALLOC_POSITION:
	{
		_data.PosCap *= 2;
		break;
	}
	case ALLOC_TECCOORD:
	{
		_data.TexCap *= 2;
		break;
	}
	case ALLOC_NORMAL:
	{
		_data.NormCap *= 2;
		break;
	}
	case ALLOC_FACE:
	{
		_data.FaceCap *= 2;
		break;
	}
	case ALLOC_SUB_MESH:
	{
		_data.SubMeshCap *= 2;
		break;
	}
	case ALLOC_SHRINK_TO_FIT:
	{
		_data.PosCap = _data.NumPos;
		_data.TexCap = _data.NumTex;
		_data.NormCap = _data.NumNorm;
		_data.FaceCap = _data.NumFace;
		_data.SubMeshCap = _data.NumSubMesh;
		break;
	}
	default:
		break;
	}
	ndat = _data;
	ndat.allocated = sizeof(Data) + 
		_data.PosCap * sizeof(Position) +
		_data.TexCap * sizeof(TexCoord) +
		_data.NormCap * sizeof(Normal) +
		_data.FaceCap * sizeof(Face) +
		_data.SubMeshCap * sizeof(SubMesh);
	ndat.buffer = operator new(ndat.allocated);

	memcpy(ndat.buffer, _data.buffer, sizeof(Data));

	_data.PosStart = sizeof(Data);
	ndatap.positions = (Position*)((size_t)ndat.buffer + ndat.PosStart);

	ndat.TexStart = ndat.PosStart + ndat.PosCap * sizeof(Position);
	ndatap.texCoords = (TexCoord*)((size_t)ndat.buffer + ndat.TexStart);

	ndat.NormStart = ndat.TexStart + ndat.TexCap * sizeof(TexCoord);
	ndatap.normals = (Normal*)((size_t)ndat.buffer + ndat.NormStart);

	ndat.FaceStart = ndat.NormStart + ndat.NormCap * sizeof(Normal);
	ndatap.faces = (Face*)((size_t)ndat.buffer + ndat.FaceStart);

	ndat.SubMeshStart = ndat.FaceStart + ndat.FaceCap * sizeof(Face);
	ndatap.subMesh = (SubMesh*)((size_t)ndat.buffer + ndat.SubMeshStart);

	memcpy(ndatap.positions, _datap.positions, ndat.NumPos * sizeof(Position));
	memcpy(ndatap.texCoords, _datap.texCoords, ndat.NumTex * sizeof(TexCoord));
	memcpy(ndatap.normals, _datap.normals, ndat.NumNorm * sizeof(Normal));
	memcpy(ndatap.faces, _datap.faces, ndat.NumFace * sizeof(Face));
	memcpy(ndatap.subMesh, _datap.subMesh, ndat.NumSubMesh * sizeof(SubMesh));

	operator delete(_data.buffer);
	_data = ndat;
	_datap = ndatap;

}

Object::Object()
{
	Alloc(ALLOC_ALL);
}
Object::Object(ifstream & file)
{
	file.read((char*)&_data, sizeof(Data));
	_data.buffer = operator new(_data.allocated);
	file.read((char*)_data.buffer, _data.allocated);
	_datap.positions = (Position*)((size_t)_data.buffer + _data.PosStart);
	_datap.texCoords = (TexCoord*)((size_t)_data.buffer + _data.TexStart);
	_datap.normals = (Normal*)((size_t)_data.buffer + _data.NormStart);
	_datap.faces = (Face*)((size_t)_data.buffer + _data.FaceStart);
	_datap.subMesh = (SubMesh*)((size_t)_data.buffer + _data.SubMeshStart);
}
Object::~Object()
{
	operator delete(_data.buffer);
}

Object & Object::operator=(Object & other)
{
	void* temp = _data.buffer;
	_data = other._data;
	other._data.buffer = temp;
	_datap = other._datap;

	return (*this);
}

void Object::AddPosition(const Position & pos)
{
	if (_data.NumPos >= _data.PosCap)
	{
		// Allocate more space
		Alloc(ALLOC_POSITION);
	}

	_datap.positions[_data.NumPos] = pos;
	_data.NumPos++;
}

void Object::AddTexCoord(const TexCoord & tex)
{
	if (_data.NumTex >= _data.TexCap)
	{
		// Allocate more space
		Alloc(ALLOC_TECCOORD);
	}

	_datap.texCoords[_data.NumTex] = tex;
	_data.NumTex++;
}

void Object::AddNormal(const Normal & norm)
{
	if (_data.NumPos >= _data.PosCap)
	{
		// Allocate more space
		Alloc(ALLOC_NORMAL);
	}

	_datap.normals[_data.NumNorm] = norm;
	_data.NumNorm++;
}

void Object::AddFace(const Face & face)
{
	if (_data.NumFace >= _data.FaceCap)
	{
		// Allocate more space.
		Alloc(ALLOC_FACE);
	}

	_datap.faces[_data.NumFace] = face;
	if(_data.NumSubMesh)
		_datap.subMesh[_data.NumSubMesh - 1].faceCount++;
	_data.NumFace++;
}

void Object::AddSubMesh(const string & name)
{
	if (_data.NumSubMesh >= _data.SubMeshCap)
	{
		// Allocate more space.
		Alloc(ALLOC_SUB_MESH);
	}

	memcpy(_datap.subMesh[_data.NumSubMesh].name, name.c_str(), name.size() + 1);
	_datap.subMesh[_data.NumSubMesh].faceStart = _data.NumFace;
	_datap.subMesh[_data.NumSubMesh].faceCount = 0;
	_data.NumSubMesh++;
}

void Object::ShrinkToFit()
{
	Alloc(ALLOC_SHRINK_TO_FIT);
}

void Object::Triangulate()
{
	Object o;

	for (uint64_t i = 0; i < _data.NumPos; i++)
	{
		o.AddPosition(_datap.positions[i]);
	}
	for (uint64_t i = 0; i < _data.NumTex; i++)
	{
		o.AddTexCoord(_datap.texCoords[i]);
	}
	for (uint64_t i = 0; i < _data.NumNorm; i++)
	{
		o.AddNormal(_datap.normals[i]);
	}
	if (_data.NumSubMesh)
	{
		for (uint32_t i = 0; i < _data.NumSubMesh; i++)
		{
			o.AddSubMesh(_datap.subMesh[i].name);
			for (uint64_t j = _datap.subMesh[i].faceStart; j < _datap.subMesh[i].faceCount + _datap.subMesh[i].faceStart; j++)
			{
				auto face = _datap.faces[j];
				switch (face.indexCount)
				{
				case 3:
				{
					o.AddFace(face);
					break;
				}
				case 4:
				{
					vector<vector<uint64_t>> nf;

					for (uint8_t k = 0; k < 3; k++)
					{
						vector<uint64_t> ind;
						for (uint8_t n = 0; n < face.indices[k].indexCount; n++)
						{
							ind.push_back(face.indices[k].index[n]);
						}
						nf.push_back(ind);
					}
					o.AddFace(Face(nf));
					nf.clear();

					uint8_t de[] = { 2,3,0 };
					for (uint8_t k = 0; k < 3; k++)
					{
						vector<uint64_t> ind;
						for (uint8_t n = 0; n < face.indices[de[k]].indexCount; n++)
						{
							ind.push_back(face.indices[de[k]].index[n]);
						}
						nf.push_back(ind);
					}
					o.AddFace(Face(nf));
					break;
				}
				default:
					break;
				}
			}
		}
	}
	else
	{
		for (uint64_t j = 0; j < _data.NumFace; j++)
		{
			auto face = _datap.faces[j];
			switch (face.indexCount)
			{
			case 3:
			{
				o.AddFace(face);
				break;
			}
			case 4:
			{
				vector<vector<uint64_t>> nf;

				for (uint8_t k = 0; k < 3; k++)
				{
					vector<uint64_t> ind;
					for (uint8_t n = 0; n < face.indices[k].indexCount; n++)
					{
						ind.push_back(face.indices[k].index[n]);
					}
					nf.push_back(ind);
				}
				o.AddFace(Face(nf));
				nf.clear();

				uint8_t de[] = { 2,3,0 };
				for (uint8_t k = 0; k < 3; k++)
				{
					vector<uint64_t> ind;
					for (uint8_t n = 0; n < face.indices[de[k]].indexCount; n++)
					{
						ind.push_back(face.indices[de[k]].index[n]);
					}
					nf.push_back(ind);
				}
				o.AddFace(Face(nf));
				break;
			}
			default:
				break;
			}
		}
	}
	(*this) = o;
}


void Object::Print()
{
	cout << "Positions: " << endl;
	for (uint64_t i = 0; i < _data.NumPos; i++)
	{
		cout << "\t" << _datap.positions[i].x << " " << _datap.positions[i].y << " " << _datap.positions[i].z/* << " " << _datap.positions[i].w*/ << endl;
	}
	cout << "Texcoords: " << endl;
	for (uint64_t i = 0; i < _data.NumTex; i++)
	{
		cout << "\t" << _datap.texCoords[i].u << " " << _datap.texCoords[i].v/* << " " << _datap.texCoords[i].w << " "*/ << endl;
	}
	cout << "Normals: " << endl;
	for (uint64_t i = 0; i < _data.NumNorm; i++)
	{
		cout << "\t" << _datap.normals[i].x << " " << _datap.normals[i].y << " " << _datap.normals[i].z << endl;
	}
	if (_data.NumSubMesh)
	{
		cout << "SubMesh: " << endl;
		for (uint32_t n = 0; n < _data.NumSubMesh; n++)
		{
			cout << "\t" << _datap.subMesh[n].name << endl;
			for (uint64_t i = _datap.subMesh[n].faceStart; i < _datap.subMesh[n].faceCount + _datap.subMesh[n].faceStart; i++)
			{
				cout << "\t\t";
				for (uint8_t j = 0; j < _datap.faces[i].indexCount; j++)
				{
					for (uint8_t k = 0; k < _datap.faces[i].indices[j].indexCount - 1; k++)
					{
						cout << _datap.faces[i].indices[j].index[k] << "/";
					}
					if (_datap.faces[i].indices[j].indexCount > 0)
						cout << _datap.faces[i].indices[j].index[_datap.faces[i].indices[j].indexCount - 1] << " ";
				}
				cout << endl;
			}
		}
	}
	else
	{
		cout << "Faces: " << endl;
		for (uint64_t i = 0; i < _data.NumFace; i++)
		{
			cout << "\t";
			for (uint8_t j = 0; j < _datap.faces[i].indexCount; j++)
			{
				for (uint8_t k = 0; k < _datap.faces[i].indices[j].indexCount - 1; k++)
				{
					cout << _datap.faces[i].indices[j].index[k] << "/";
				}
				if (_datap.faces[i].indices[j].indexCount > 0)
					cout << _datap.faces[i].indices[j].index[_datap.faces[i].indices[j].indexCount - 1] << " ";
			}
			cout << endl;
		}
	}
}

void Object::Clear()
{
	Alloc(ALLOC_ALL);
}

Data & Object::GetData()
{
	return _data;
}

DataPointers & Object::GetDataP()
{
	return _datap;
}

void Object::GenerateArf(ofstream & of, int flag)
{
	if (flag & GENERATE_ARF_FLAG_SHRINK_TO_FIT)
		ShrinkToFit();
	of.write((char*)&_data, sizeof(ArfData::Data));
	of.write((char*)_data.buffer, _data.allocated);
}

