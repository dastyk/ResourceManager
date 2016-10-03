#ifndef _ASSET_PARSER_H_
#define _ASSET_PARSER_H_

#pragma once
#include "Resource.h"
#include "ArfData.h"
#include <functional>
#include <map>
#include "Types.h"
class AssetParser
{
	std::map<uint32_t, std::function<void(Resource & r)>> _parsers;
public:
	AssetParser();
	~AssetParser();

	void AddParser(uint32_t type,const std::function<void(Resource & r)>& parseFunction);
	void ParseResource(Resource& r)const;
};

#endif
