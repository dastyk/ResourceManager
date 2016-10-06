#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_


#pragma once
#include "SM_GUID.h"
#include <DirectXMath.h>
#include <vector>
#include "ResourceManager.h"

class GameObject
{

	
	
	std::vector<std::pair<bool, SM_GUID>> _MeshLODs;
	std::vector<std::pair<bool, SM_GUID>> _TextureLODs;
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
		bool render = false;
		while (level >= 0)
		{
			if (ResourceManager::Instance().IsLoaded(_MeshLODs[level].second))
			{
				mesh = _MeshLODs[level].second;
				render = true;
				level = -1;
			}
			
			level--;
		}

		if (!render)
		{

			level = (int32_t)_currentLOD;
			while (level <= 5)
			{
				if (ResourceManager::Instance().IsLoaded(_MeshLODs[level].second))
				{
					mesh = _MeshLODs[level].second;
					render = true;
					level = 6;
				}

				level++;
			}
			if (!render)
			{
				if (!_MeshLODs[0].first)
				{
					_MeshLODs[0].first = true;
					ResourceManager::Instance().LoadResource(_MeshLODs[0].second, Resource::Flag::NEEDED_NOW);
				}
				if (!_MeshLODs[_currentLOD].first)
				{
					_MeshLODs[_currentLOD].first = true;
					ResourceManager::Instance().LoadResource(_MeshLODs[_currentLOD].second, Resource::Flag::NEEDED_NOW);
				}

				
			}
		}

		level = (int32_t)_currentLOD;
		while (level >= 0)
		{
			if (ResourceManager::Instance().IsLoaded(_TextureLODs[level].second))
			{
				texture = _TextureLODs[level].second;
				
				return render;
			}

			level--;
		}
		level = (int32_t)_currentLOD;
		while (level <= 5)
		{
			if (ResourceManager::Instance().IsLoaded(_TextureLODs[level].second))
			{
				texture = _TextureLODs[level].second;

				return render;
			}

			level++;
		}
		if (!_TextureLODs[0].first)
		{
			_TextureLODs[0].first = true;
			ResourceManager::Instance().LoadResource(_TextureLODs[0].second, Resource::Flag::NEEDED_NOW);
		}	
		if (!_TextureLODs[_currentLOD].first)
		{
			_TextureLODs[_currentLOD].first = true;
			ResourceManager::Instance().LoadResource(_TextureLODs[_currentLOD].second, Resource::Flag::NEEDED_NOW);
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
					if (!_MeshLODs[i].first)
					{
						_MeshLODs[i].first = true;
					
						ResourceManager::Instance().LoadResource(_MeshLODs[i].second, Resource::Flag::NEEDED_NOW);
						
					}
					if (!_TextureLODs[i].first)
					{
						_TextureLODs[i].first = true;
						ResourceManager::Instance().LoadResource(_TextureLODs[i].second, Resource::Flag::NEEDED_NOW);
					}
				}
				else if ((i == LOD - 1 || i == LOD + 1))
				{
					if (!_MeshLODs[i].first)
					{
						_MeshLODs[i].first = true;
						ResourceManager::Instance().LoadResource(_MeshLODs[i].second, Resource::Flag::NOT_URGENT);
					}
					if (!_TextureLODs[i].first)
					{
						_TextureLODs[i].first = true;					
						ResourceManager::Instance().LoadResource(_TextureLODs[i].second, Resource::Flag::NOT_URGENT);
					}
				}
				else
				{
					if (_MeshLODs[i].first)
					{
						_MeshLODs[i].first = false;
						ResourceManager::Instance().UnloadResource(_MeshLODs[i].second);
					}
					if (_TextureLODs[i].first)
					{
						_TextureLODs[i].first = false;
						ResourceManager::Instance().UnloadResource(_TextureLODs[i].second);
					}
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
		_MeshLODs.push_back(std::pair<bool, SM_GUID>(false, guid));
	}
	void AddLODTexture(SM_GUID guid)
	{
		//texture = guid;
		//ResourceManager::Instance().LoadResource(guid, Resource::Flag::LOAD_AND_WAIT);
		_TextureLODs.push_back(std::pair<bool, SM_GUID>(false, guid));
	}
};

#endif