#include "HubLevelManager.h"

#include "../../../Objects/Tags/ObjectTagManager.h"
#include "../../../Resources/Save/SaveManager.h"


#define HUB_L2_PLAYER_HOUSE_TAG "L2PlayerHouse"
#define HUB_L3_PLAYER_HOUSE_TAG "L3PlayerHouse"

namespace GAME_NAME::Level
{
	using namespace GAME_NAME::Resources;

	HubLevelManager::HubLevelManager()
	{
		m_currentHubCharacteristics.Load();

		updateHouseUnlock();
	}

	void HubLevelManager::Update(GLFWwindow* window)
	{

	}

	void HubLevelManager::updateHouseUnlock()
	{
		auto [beginl2, endl2] = Tags::ObjectTagManager::GetObjectsWithTag(HUB_L2_PLAYER_HOUSE_TAG);

		for (beginl2; beginl2 != endl2; ++beginl2)
		{
			beginl2->second->SetActive(m_currentHubCharacteristics.PlayerHouseLevel.second >= 2);
		}

		auto [beginl3, endl3] = Tags::ObjectTagManager::GetObjectsWithTag(HUB_L3_PLAYER_HOUSE_TAG);

		for (beginl3; beginl3 != endl3; ++beginl3)
		{
			beginl3->second->SetActive(m_currentHubCharacteristics.PlayerHouseLevel.second >= 3);
		}
	}

	void HubLevelManager::HubCharactersitics::Load()
	{
		SaveManager::GetSaveInteger(PlayerHouseLevel.first, PlayerHouseLevel.second);
	}

	void HubLevelManager::HubCharactersitics::Save()
	{
		SaveManager::SaveInteger(PlayerHouseLevel.second, PlayerHouseLevel.first);
	}
}

