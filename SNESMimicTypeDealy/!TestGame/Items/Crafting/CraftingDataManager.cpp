#include "CraftingDataManager.h"

namespace GAME_NAME::Items::Crafting
{
	std::array<const Recipe, CRAFTING_DATA_NUM_RECIPES> CraftingDataManager::m_recipes = {
		Recipe { ITEM_TYPE::SCRAP_AXE, std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::APPLE, 1 }, { ITEM_TYPE::LOG, 1 } } },
		Recipe { ITEM_TYPE::APPLE, std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::LOG, 2 } } },
		Recipe { ITEM_TYPE::WOODEN_SHOES, std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::LOG, 1 }, { ITEM_TYPE::LEAVES, 1 }, { ITEM_TYPE::WOODEN_PLANK, 1 } } },
	};
}
