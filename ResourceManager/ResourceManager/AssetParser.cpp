#include "AssetParser.h"



AssetParser::AssetParser()
{
}


AssetParser::~AssetParser()
{
}

void AssetParser::ParseResource(Resource & r) const
{
	switch (r._objectType)
	{
	case ObjectType::Mesh:
	{
		ParseMesh(r);
		break;
	}
	case ObjectType::Texture:
	{
		ParseTexture(r);
		break;
	}
	default:
		throw std::runtime_error("Unable to handle object type.");
		break;
	}
}

void AssetParser::ParseMesh(Resource & r) const
{
	switch (r._fileType)
	{
	case FileType::obj:
	{
		ParseObj(r);
		break;
	}
	default:
		throw std::runtime_error("Unable to handle mesh type.");
		break;
	}
}

void AssetParser::ParseTexture(Resource & r) const
{
}

void AssetParser::ParseObj(Resource & r) const
{
}

void AssetParser::ParseArf(Resource & r) const
{
	ArfData::Data* data = (ArfData::Data*)r._rawData;
	ArfData::DataPointers _datap;
	_datap.positions = (ArfData::Position*)((size_t)r._rawData + data->PosStart);
	_datap.texCoords = (ArfData::TexCoord*)((size_t)r._rawData + data->TexStart);
	_datap.normals = (ArfData::Normal*)((size_t)r._rawData + data->NormStart);
	_datap.faces = (ArfData::Face*)((size_t)r._rawData + data->FaceStart);
	_datap.subMesh = (ArfData::SubMesh*)((size_t)r._rawData + data->SubMeshStart);


	// TODO: Do something with the data, so it can be rendered. // TODO: Interleave data.
}
