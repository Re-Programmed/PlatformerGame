#pragma once
#include <string>
#include <vector>
#include "../GameObject.h"

namespace GAME_NAME::Objects::Instantiate
{
	class LevelObjectHandler
	{
	public:
		static GameObject* GetLevelObject(std::string parent, std::string code, bool spawnObject = false, std::string levelSource = "");

		static std::vector<GameObject*> GetLevelObjects(std::string parent);
	};
}