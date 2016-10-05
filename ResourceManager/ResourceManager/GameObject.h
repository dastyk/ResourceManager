#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_


#pragma once
#include "SM_GUID.h"
#include <DirectXMath.h>
#include <vector>
#include "ResourceManager.h"

class GameObject
{

	
	
	std::vector<std::pair<bool, SM_GUID>> _LODs;
	uint32_t _currentLOD;
public:
	SM_GUID mesh;
	SM_GUID texture;

	DirectX::XMFLOAT4X4 transform;

	DirectX::XMFLOAT3 pos;
	float scale;
	float radius;

	GameObject()
	{

	}
	~GameObject()
	{

	}
	bool GetLODGUIDToRender()
	{
		int32_t level = (int32_t)_currentLOD;
		while (level >= 0)
		{
			if (ResourceManager::Instance().IsLoaded(_LODs[level].second))
			{
				mesh = _LODs[level].second;
				return true;
			}
			
			level--;
		}
		
		if (!_LODs[5].first)
		{
			_LODs[5].first = true;
			ResourceManager::Instance().LoadResource(_LODs[5].second, Resource::Flag::NEEDED_NOW);
		}
			
		if (!_LODs[0].first)
		{
			_LODs[0].first = true;
			ResourceManager::Instance().LoadResource(_LODs[0].second, Resource::Flag::NEEDED_NOW);
		}

		return false;
			
	}
	int32_t GetCurrentLOD() { return _currentLOD; }
	void SetCurrentLOD(uint32_t LOD) 
	{ 
		if (_currentLOD != LOD)
		{
			for (uint8_t i = 0; i < 6; i++)
			{
				if (i == LOD)
				{
					if (!_LODs[i].first)
					{
						_LODs[i].first = true;
						ResourceManager::Instance().LoadResource(_LODs[i].second, Resource::Flag::NEEDED_NOW);
					}
				}
				else if ((i == LOD - 1 || i == LOD + 1))
				{
					if (!_LODs[i].first)
					{
						_LODs[i].first = true;
						ResourceManager::Instance().LoadResource(_LODs[i].second, Resource::Flag::NOT_URGENT);
					}
				}
				else if (_LODs[i].first)
				{
					_LODs[i].first = false;
					ResourceManager::Instance().UnloadResource(_LODs[i].second);
				}

			}
			_currentLOD = LOD;
		}
	}
	float GetRadius() { return radius; }
	float GetScale() { return scale; }
	DirectX::XMFLOAT3& GetPos() { return pos; }
	void AddLODMesh(SM_GUID guid)
	{
		_LODs.push_back(std::pair<bool, SM_GUID>(false, guid));
	}
	void AddLODTexture(SM_GUID guid)
	{
		texture = guid;
		ResourceManager::Instance().LoadResource(guid, Resource::Flag::LOAD_AND_WAIT);
	}
};

#endif