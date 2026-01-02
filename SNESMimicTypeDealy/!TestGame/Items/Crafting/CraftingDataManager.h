#pragma once

#include "../../Items/ItemType.h"

namespace GAME_NAME::Items::Crafting
{
#define CRAFTING_DATA_NUM_RECIPES 3

	struct Recipe
	{
		ITEM_TYPE Return;
		std::unordered_map<ITEM_TYPE, int> Inputs;
	};

	class CraftingDataManager
	{
	public:
		inline static const Recipe& GetRecipe(const int& index)
		{
			return m_recipes[index];
		}
	private:
		static std::array<const Recipe, CRAFTING_DATA_NUM_RECIPES> m_recipes;
	};
}