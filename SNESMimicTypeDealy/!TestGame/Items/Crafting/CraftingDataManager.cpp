#include "CraftingDataManager.h"

namespace GAME_NAME::Items::Crafting
{
	std::array<const Recipe, CRAFTING_DATA_NUM_RECIPES> CraftingDataManager::m_recipes = {
		Recipe { SCRAP_AXE, std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::STONE, 1 }, { ITEM_TYPE::LOG, 1 } } }
	};
}
