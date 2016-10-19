#ifndef _ASSET_PARSER_H_
#define _ASSET_PARSER_H_

#pragma once

#include <functional>
#include <map>
#include "Types.h"
#include "Resource.h"

class AssetParser
{
	std::map<uint8_t, std::function<void(const Resource::Ptr& resourcePtr)>> _parsers;
public:
	AssetParser();
	~AssetParser();

	void AddParser(uint8_t type,const std::function<void(const Resource::Ptr& resourcePtr)>& parseFunction);
	void ParseResource(const Resource::Ptr& resourcePtr)const;
};

#endif
