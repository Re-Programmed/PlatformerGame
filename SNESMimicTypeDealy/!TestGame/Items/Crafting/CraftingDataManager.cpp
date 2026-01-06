#include "CraftingDataManager.h"

#include "../../Items/Types/Tool.h"
#include "../../Items/Types/Food.h"

namespace GAME_NAME::Items::Crafting
{
	CraftingDataManager* CraftingDataManager::INSTANCE = nullptr;

	std::array<Recipe, CRAFTING_DATA_NUM_RECIPES> CraftingDataManager::m_recipes = {
		Recipe(new Tool(ITEM_TYPE::SCRAP_AXE, 20), std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::APPLE, 1 }, {ITEM_TYPE::LOG, 1} }, true),
		Recipe(new Food(ITEM_TYPE::APPLE, 20), std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::LOG, 2 } }, false)
	};

	MiscState::SaveParam Recipe::Encode()
	{
		SaveParam param(Unlocked ? "t" : "f");
		return param;
	}

	void Recipe::Decode(const SaveParam param)
	{
		Unlocked = param.starts_with('t');
	}
}
