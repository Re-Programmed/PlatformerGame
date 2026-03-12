#pragma once
#include "../LevelSystem.h"

#include "../../../Objects/GameObject.h"
#include "../../Cutscenes/Character/AnimatingCharacter.h"
#include "../../Objects/Player/Player.h"

#include <array>

namespace GAME_NAME::Level
{
	class GreenRegion2LevelManager
		: public LevelSystem
	{
	public:
		GreenRegion2LevelManager();

		void Update(GLFWwindow* window) override;

	private:
		static Cutscenes::AnimatingCharacter* m_evilWolf;
		static bool m_playedEvilWolfCutscene, m_wolfJumped;
	};
}