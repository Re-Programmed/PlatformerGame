#pragma once
#include "../LevelSystem.h"

#include "../../../Objects/GameObject.h"

#include "../../Objects/Player/Player.h"

#include <array>

namespace GAME_NAME::Level
{
	class EmptyLevelSystem
		: public LevelSystem
	{
	public:
		EmptyLevelSystem()
		{

		}

		void Update(GLFWwindow* window) override
		{

		}
	};
}