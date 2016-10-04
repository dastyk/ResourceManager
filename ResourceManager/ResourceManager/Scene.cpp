#include "Scene.h"

float Scene::DistanceBetweenPoints(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::AddGameObject(GameObject gameObject)
{
	gameObjects.push_back(gameObject);
}

void Scene::Update(DirectX::XMFLOAT3 playerPos)
{
	bool usedLevels[6];

	for (int i = 0; i < gameObjects.size(); i++)
	{

		float dist = DistanceBetweenPoints(playerPos, gameObjects[i].pos);

		if (dist <= gameObjects[i].radius * gameObjects[i].scale + 5.0f)
		{
			gameObjects[i].mesh = SM_GUID("Sphere5.arf");
			usedLevels[5] = true;
		}
		else if (dist <= gameObjects[i].radius * gameObjects[i].scale + 10.0f)
		{
			gameObjects[i].mesh = SM_GUID("Sphere4.arf");
			usedLevels[4] = true;
		}
		else if (dist <= gameObjects[i].radius * gameObjects[i].scale + 15.0f)
		{
			gameObjects[i].mesh = SM_GUID("Sphere3.arf");
			usedLevels[3] = true;
		}
		else if (dist <= gameObjects[i].radius * gameObjects[i].scale + 20.0f)
		{
			gameObjects[i].mesh = SM_GUID("Sphere2.arf");
			usedLevels[2] = true;
		}
		else if (dist <= gameObjects[i].radius * gameObjects[i].scale + 25.0f)
		{
			gameObjects[i].mesh = SM_GUID("Sphere1.arf");
			usedLevels[1] = true;
		}
		else if (dist <= gameObjects[i].radius * gameObjects[i].scale + 30.0f)
		{
			gameObjects[i].mesh = SM_GUID("Sphere0.arf");
			usedLevels[0] = true;
		}
	}

	if (usedLevels[0])
	{
		ResourceManager::Instance().LoadResource("Sphere0.arf", Resource::Flag::NEEDED_NOW);
		ResourceManager::Instance().LoadResource("Sphere1.arf", Resource::Flag::NOT_URGENT);

		usedLevels[1] = true;
	}

	if (usedLevels[1])
	{
		ResourceManager::Instance().LoadResource("Sphere1.arf", Resource::Flag::NEEDED_NOW);
		ResourceManager::Instance().LoadResource("Sphere2.arf", Resource::Flag::NOT_URGENT);
		ResourceManager::Instance().LoadResource("Sphere0.arf", Resource::Flag::NOT_URGENT);

		usedLevels[2] = true;
		usedLevels[0] = true;
	}

	if (usedLevels[2])
	{
		ResourceManager::Instance().LoadResource("Sphere2.arf", Resource::Flag::NEEDED_NOW);
		ResourceManager::Instance().LoadResource("Sphere3.arf", Resource::Flag::NOT_URGENT);
		ResourceManager::Instance().LoadResource("Sphere1.arf", Resource::Flag::NOT_URGENT);

		usedLevels[3] = true;
		usedLevels[1] = true;
	}

	if (usedLevels[3])
	{
		ResourceManager::Instance().LoadResource("Sphere3.arf", Resource::Flag::NEEDED_NOW);
		ResourceManager::Instance().LoadResource("Sphere4.arf", Resource::Flag::NOT_URGENT);
		ResourceManager::Instance().LoadResource("Sphere2.arf", Resource::Flag::NOT_URGENT);

		usedLevels[4] = true;
		usedLevels[2] = true;
	}

	if (usedLevels[4])
	{
		ResourceManager::Instance().LoadResource("Sphere4.arf", Resource::Flag::NEEDED_NOW);
		ResourceManager::Instance().LoadResource("Sphere5.arf", Resource::Flag::NOT_URGENT);
		ResourceManager::Instance().LoadResource("Sphere3.arf", Resource::Flag::NOT_URGENT);

		usedLevels[5] = true;
		usedLevels[3] = true;
	}

	if (usedLevels[5])
	{
		ResourceManager::Instance().LoadResource("Sphere5.arf", Resource::Flag::NEEDED_NOW);
		ResourceManager::Instance().LoadResource("Sphere4.arf", Resource::Flag::NOT_URGENT);

		usedLevels[4] = true;
	}

	for(int i = 0; i < 6; i++)
	{
		if (!usedLevels[i])
		{
			//ResourceManager::Instance().DESTROY!!!
		}
	}

}

std::vector<GameObject>& Scene::GetObjectsToRender()
{
	return gameObjects;
}
