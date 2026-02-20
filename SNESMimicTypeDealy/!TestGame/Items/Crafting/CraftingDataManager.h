#pragma once

#include "../../Items/ItemType.h"

#include "../../Items/InventoryItem.h"

#include "../../../Objects/MiscStateGroup.h"

namespace GAME_NAME::Items::Crafting
{
#define CRAFTING_DATA_NUM_RECIPES 5

	class Recipe
		: public MiscState
	{
	public:
		Recipe(InventoryItem* returnItem, std::unordered_map<ITEM_TYPE, int> inputs, bool defaultUnlocked = false)
			: Return(returnItem), Inputs(inputs), Unlocked(defaultUnlocked)
		{
			
		}

		Recipe()
			: Return(nullptr), Inputs(), Unlocked(false)
		{

		}

		InventoryItem* Return;
		std::unordered_map<ITEM_TYPE, int> Inputs;

		bool Unlocked SERIALIZED;

		SaveParam Encode() override;
		void Decode(const SaveParam param) override;
	};

	class CraftingDataManager
		: public MiscStateGroup
	{
	public:
		CraftingDataManager()
			: MiscStateGroup("craft_data")
		{
			std::vector<std::string>* states = getStates().get();
			if (states->size() >= CRAFTING_DATA_NUM_RECIPES)
			{
				for (int i = 0; i < CRAFTING_DATA_NUM_RECIPES; i++)
				{
					m_recipes[i].Decode((*states)[i]);
				}
			}

			for (size_t i = 0; i < m_recipes.size(); i++)
			{
				assignState(&m_recipes[i]);
			}
		}

		inline static void Initilize()
		{
			if (INSTANCE == nullptr)
			{
				INSTANCE = new CraftingDataManager();
			}
		}

		inline static Recipe& GetRecipe(const int& index)
		{
			return m_recipes[index];
		}
	private:
		static CraftingDataManager* INSTANCE;

		static std::array<Recipe, CRAFTING_DATA_NUM_RECIPES> m_recipes;
	};
}