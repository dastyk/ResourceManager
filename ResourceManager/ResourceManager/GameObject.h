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
	//uint32_t _currentLOD;
	uint32_t _currentTextureLOD = 0;
	uint32_t _currentMeshLOD = 0;
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
		int32_t level = (int32_t)_currentMeshLOD;
		bool render = false;
		while (level >= 0)
		{
			if (ResourceManager::Instance().IsLoaded(_MeshLODs[level].second))
			{
				mesh = _MeshLODs[level].second;
				render = true;
				level = -1;
			}
			else
			{
				if (!_MeshLODs[_currentMeshLOD].first)
				{
					_MeshLODs[_currentMeshLOD].first = true;
					ResourceManager::Instance().LoadResource(_MeshLODs[_currentMeshLOD].second, Resource::Flag::NEEDED_NOW);
				}
				if (!_MeshLODs[0].first)
				{
					_MeshLODs[0].first = true;
					ResourceManager::Instance().LoadResource(_MeshLODs[0].second, Resource::Flag::NEEDED_NOW);
				}
				

			}
			
			level--;
		}
		
		if (render)
		{
			render = false;
			level = (int32_t)_currentTextureLOD;
			while (level >= 0)
			{
				if (ResourceManager::Instance().IsLoaded(_TextureLODs[level].second))
				{
					texture = _TextureLODs[level].second;
					render = true;
					level = -1;
				}
				else
				{
					if (!_TextureLODs[_currentTextureLOD].first)
					{
						_TextureLODs[_currentTextureLOD].first = true;
						ResourceManager::Instance().LoadResource(_TextureLODs[_currentTextureLOD].second, Resource::Flag::NEEDED_NOW);
					}
					if (!_TextureLODs[0].first)
					{
						_TextureLODs[0].first = true;
						ResourceManager::Instance().LoadResource(_TextureLODs[0].second, Resource::Flag::NEEDED_NOW);
					}
					
				}

				level--;
			}
		}

		return render;
			
	}
	int32_t GetCurrentMeshLOD() { return _currentMeshLOD; }
	int32_t GetCurrentTextureLOD() { return _currentTextureLOD; }
	void SetCurrentMeshLOD(uint32_t LOD) 
	{ 
		if (_currentMeshLOD != LOD)
		{
			for (uint8_t i = 0; i < _MeshLODs.size(); i++)
			{
				if (i == LOD)
				{
					if (!_MeshLODs[i].first)
					{
						_MeshLODs[i].first = true;
					
						ResourceManager::Instance().LoadResource(_MeshLODs[i].second, Resource::Flag::NEEDED_NOW);
						
					}
				}
				else if ((i == LOD - 1 || i == LOD + 1))
				{
					if (!_MeshLODs[i].first)
					{
						_MeshLODs[i].first = true;
						ResourceManager::Instance().LoadResource(_MeshLODs[i].second, Resource::Flag::NOT_URGENT);
					}
				}
				else
				{
					if (_MeshLODs[i].first)
					{
						_MeshLODs[i].first = false;
						ResourceManager::Instance().UnloadResource(_MeshLODs[i].second);
					}
				}

			}
			_currentMeshLOD = LOD;
		}
	}

	void SetCurrentTextureLOD(uint32_t LOD)
	{
		if (_currentTextureLOD != LOD)
		{
			for (uint8_t i = 0; i < _TextureLODs.size(); i++)
			{
				if (i == LOD)
				{
					if (!_TextureLODs[i].first)
					{
						_TextureLODs[i].first = true;
						ResourceManager::Instance().LoadResource(_TextureLODs[i].second, Resource::Flag::NEEDED_NOW);
					}
				}
				else if ((i == LOD - 1 || i == LOD + 1))
				{
					if (!_TextureLODs[i].first)
					{
						_TextureLODs[i].first = true;
						ResourceManager::Instance().LoadResource(_TextureLODs[i].second, Resource::Flag::NOT_URGENT);
					}
				}
				else
				{
					if (_TextureLODs[i].first)
					{
						_TextureLODs[i].first = false;
						ResourceManager::Instance().UnloadResource(_TextureLODs[i].second);
					}
				}

			}
			_currentTextureLOD = LOD;
		}
	}
	float GetRadius() { return radius; }
	float GetScale() { return scale; }
	uint32_t GetNrOfMeshLODLevels() const { return _MeshLODs.size(); }
	uint32_t GetNrOfTextureLODLevels() const { return _TextureLODs.size(); }
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