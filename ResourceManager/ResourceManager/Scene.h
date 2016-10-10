#pragma once
#ifndef SCENE_H
#define SCENE_H
#pragma once

#include <vector>

#include "GameObject.h"
#include "ResourceManager.h"

#include <math.h>

class Scene
{
private:
	std::vector<GameObject> gameObjects;

	float DistanceBetweenPoints(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);

public:
	Scene();
	virtual ~Scene();

	void AddGameObject(GameObject& gameObject);
	void Update(DirectX::XMFLOAT3 playerPos);

	void GetObjectsToRender(std::vector<GameObject>& objects);
};

#endif