#pragma once
#include "../LevelSystem.h"

#include "../../../Objects/GameObject.h"

#include "../../Objects/Player/Player.h"

#include <array>

namespace GAME_NAME::Level
{
	class HubLevelManager
		: public LevelSystem
	{
	public:
		HubLevelManager();

		void Update(GLFWwindow* window) override;

		inline void SetUnlockHouseLevel(const int& level)
		{
			m_currentHubCharacteristics.PlayerHouseLevel.second = level;
			m_currentHubCharacteristics.Save();
			updateHouseUnlock();
		}

		inline int GetUnlockHouseLevel()
		{
			return m_currentHubCharacteristics.PlayerHouseLevel.second;
		}

	private:

		/// <summary>
		/// Updates what objects are enabled in the hub to display the house at the current unlock level.
		/// </summary>
		void updateHouseUnlock();

		struct HubCharactersitics
		{
			std::pair<std::string, int> PlayerHouseLevel = std::make_pair("PlayerHouseLevel", 1);

			void Load();
			void Save();
		};

		HubCharactersitics m_currentHubCharacteristics;

	};
}