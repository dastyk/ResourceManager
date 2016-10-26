#include "Scene.h"
#include "Core.h"
float Scene::DistanceBetweenPoints(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	//return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::AddGameObject(GameObject& gameObject)
{
	gameObjects.push_back(gameObject);
}

void Scene::Update(DirectX::XMFLOAT3 playerPos)
{
	for (int i = 0; i < gameObjects.size(); i++)
	{

		uint32_t meshlods = gameObjects[i].GetNrOfMeshLODLevels();
		uint32_t texlods = gameObjects[i].GetNrOfTextureLODLevels();
		float farplane = Core::GetInstance()->GetCameraManager()->GetFarPlaneDistance()*0.5f;
		float meshStep = farplane / meshlods;
		float texStep = farplane / texlods;
		float dist = DistanceBetweenPoints(playerPos, gameObjects[i].GetPos());
		for (uint32_t j = 0; j < meshlods; j++)
		{
			if (dist <= gameObjects[i].GetRadius() * gameObjects[i].GetScale() + (j + 1) * meshStep)
			{
				gameObjects[i].SetCurrentMeshLOD(meshlods - j - 1);
				break;
			}
		}

		for (uint32_t j = 0; j < texlods; j++)
		{
			if (dist <= gameObjects[i].GetRadius() * gameObjects[i].GetScale() + (j + 1) * texStep)
			{
				gameObjects[i].SetCurrentTextureLOD(texlods - j - 1);
				break;
			}
		}
		

		//if (dist <= gameObjects[i].GetRadius() * gameObjects[i].GetScale() + 5.0f)
		//{
		//	gameObjects[i].SetCurrentLOD(5);
		//}
		//else if (dist <= gameObjects[i].GetRadius() * gameObjects[i].GetScale() + 10.0f)
		//{
		//	gameObjects[i].SetCurrentLOD(4);
		//}
		//else if (dist <= gameObjects[i].GetRadius() * gameObjects[i].GetScale() + 15.0f)
		//{
		//	gameObjects[i].SetCurrentLOD(3);
		//}
		//else if (dist <= gameObjects[i].GetRadius() * gameObjects[i].GetScale() + 20.0f)
		//{
		//	gameObjects[i].SetCurrentLOD(2);
		//}
		//else if (dist <= gameObjects[i].GetRadius() * gameObjects[i].GetScale() + 25.0f)
		//{
		//	gameObjects[i].SetCurrentLOD(1);
		//}
		//else if (dist <= gameObjects[i].GetRadius() * gameObjects[i].GetScale() + 30.0f)
		//{
		//	gameObjects[i].SetCurrentLOD(0);
		//}
	}
}

void Scene::GetObjectsToRender(std::vector<GameObject>& objects)
{
	for (auto& o : gameObjects)
	{
		if(o.GetLODGUIDToRender())
			objects.push_back(o);
	}
}
