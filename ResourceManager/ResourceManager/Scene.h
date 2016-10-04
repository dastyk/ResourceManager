#pragma once
#ifndef SCENE_H
#define SCENE_H
#pragma once

#include <vector>

#include "Structs.h"
#include "ResourceManager.h"

#include <math.h>

class Scene
{
private:
	std::vector<GameObject> gameObjects;

	float DistanceBetweenPoints(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

public:
	Scene();
	virtual ~Scene();

	void AddGameObject(GameObject gameObject);
	void Update(DirectX::XMFLOAT3 playerPos);

	std::vector<GameObject>& GetObjectsToRender();
};

#endif