#include "CraftingDataManager.h"

#include "../../Items/Types/Tool.h"
#include "../../Items/Types/Food.h"
#include "../../Items/Types/Placeable.h"

namespace GAME_NAME::Items::Crafting
{
	CraftingDataManager* CraftingDataManager::INSTANCE = nullptr;

	std::array<Recipe, CRAFTING_DATA_NUM_RECIPES> CraftingDataManager::m_recipes = {
		Recipe(new Tool(ITEM_TYPE::SCRAP_AXE, 20), std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::SMALL_STONE, 1 }, {ITEM_TYPE::TWIG, 2 } }, true),

		Recipe(new InventoryItem(ITEM_TYPE::WOODEN_PLATING), std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::LOG, 2 }, { ITEM_TYPE::SCREW, 1 } }, false),
		Recipe(new InventoryItem(ITEM_TYPE::LEAF_HELMET), std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::LEAVES, 3 } }, false),

		Recipe(new Placeable(ITEM_TYPE::WOODEN_PLANK), std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::LOG, 3 } }, false),

		Recipe(new Placeable(ITEM_TYPE::LEVEL_SAVE_POINT), std::unordered_map<ITEM_TYPE, int>{ { ITEM_TYPE::CLOTH, 2 }, { ITEM_TYPE::TWIG, 1 } }, true),

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
