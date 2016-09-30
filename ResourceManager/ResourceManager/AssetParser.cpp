#include "AssetParser.h"
#include "ArfData.h"


AssetParser::AssetParser()
{
}


AssetParser::~AssetParser()
{
}

void AssetParser::AddParser(uint32_t type,const std::function<void(Resource & r)>& parseFunction)
{
	_parsers[type] = std::move(parseFunction);
}

void AssetParser::ParseResource(Resource & r) const
{

	auto find = _parsers.find(r._rawData.fType);
	if(find != _parsers.end())
	{
		find->second(r);
		return;
	}

	throw std::runtime_error("Unable to handle object type.");

}

void AssetParser::ParseTexture(Resource & r) const
{
}

void AssetParser::ParseObj(Resource & r) const
{
}

void AssetParser::ParseArf(Resource & r) const
{
	ArfData::Data* data = (ArfData::Data*)r._rawData.data;
	ArfData::DataPointers _datap;
	void* pdata = r._rawData.data + sizeof(ArfData::Data);
	_datap.positions = (ArfData::Position*)((size_t)pdata + data->PosStart);
	_datap.texCoords = (ArfData::TexCoord*)((size_t)pdata + data->TexStart);
	_datap.normals = (ArfData::Normal*)((size_t)pdata + data->NormStart);
	_datap.faces = (ArfData::Face*)((size_t)pdata + data->FaceStart);
	_datap.subMesh = (ArfData::SubMesh*)((size_t)pdata + data->SubMeshStart);

	
	// TODO: Do something with the data, so it can be rendered. // TODO: Interleave data.

	delete r._rawData.data;
}
