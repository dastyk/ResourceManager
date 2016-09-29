#ifndef _ASSET_PARSER_H_
#define _ASSET_PARSER_H_

#pragma once
#include "Resource.h"
#include "ArfData.h"

class AssetParser
{
public:
	AssetParser();
	~AssetParser();

	void ParseResource(Resource& r)const;
	void ParseMesh(Resource& r)const;
	void ParseTexture(Resource& r)const;
	void ParseObj(Resource&r)const;
	void ParseArf(Resource&r)const;
};

#endif
