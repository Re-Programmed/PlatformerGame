#pragma once
#include "../LevelSystem.h"

#include "../../../Objects/GameObject.h"

#include "../../Objects/Player/Player.h"

#include <array>

namespace GAME_NAME::Level
{
	class GreenRegionLevelManager
		: public LevelSystem
	{
	public:
		GreenRegionLevelManager();

		void Update(GLFWwindow* window) override;
	};
}