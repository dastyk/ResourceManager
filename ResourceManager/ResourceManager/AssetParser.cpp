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
	RawData& rdata = *(RawData*)r.GetData();
	auto find = _parsers.find(rdata.fType);
	if(find != _parsers.end())
	{
		find->second(r);
		return;
	}

	throw std::runtime_error("Unable to handle object type.");

}
