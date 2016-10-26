#include "AssetParser.h"

AssetParser::AssetParser()
{
}


AssetParser::~AssetParser()
{
}

void AssetParser::AddParser(uint8_t type,const std::function<void(const Resource::Ptr& resourcePtr)>& parseFunction)
{
	_parsers[type] = std::move(parseFunction);
}

void AssetParser::ParseResource(const Resource::Ptr& resourcePtr) const
{
	auto& find = _parsers.find(resourcePtr.type);
	if (find != _parsers.end())
	{
		find->second(resourcePtr);
		return;
	}

	throw std::runtime_error("Unable to handle object type.");

}
