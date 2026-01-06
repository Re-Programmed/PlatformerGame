#pragma once
#include "../../../Objects/GUI/StaticGUIElement.h"
#include "../../../Objects/GUI/GUIButton.h"
#include "../../../Objects/GUI/GUIScrollArea.h"
#include "../ItemType.h"

#include "./CraftingDataManager.h"
#include "../Inventories/ItemSelectionSlot.h"

#include "../../../Utils/RemoveableUpdateable.h"

namespace GAME_NAME::Items::Crafting
{
#define CRAFTING_MENU_MANAGER_RECIPE_ELEMENT_COUNT 5

using namespace GUI;

	class CraftingMenuManager
		: public Utils::RemoveableUpdateable
	{
	public:
		static void OpenCraftingMenu();

//Windows has a macro called "max." (yay)
#pragma push_macro("max")
#undef max
		static bool CloseCraftingMenu(int maxRecipeClear = std::numeric_limits<int>::max());
#pragma pop_macro("max")

		~CraftingMenuManager()
		{
			Utils::RemoveableUpdateable::~RemoveableUpdateable();
		}

		void Update(GLFWwindow* window);

		static void AttemptAutofillRecipe(const ITEM_TYPE& in1, const ITEM_TYPE& in2, const ITEM_TYPE& in3);

	protected:
		bool GetShouldEnableOnStart()
		{
			return true;
		}
	private:
		static CraftingMenuManager* m_currentManager;

		static StaticGUIElement* const m_backgroundCover;

		static GUIScrollArea* m_recipeDisplayListing;

		struct RecipeDisplay
		{
			StaticGUIElement* Input1 = nullptr;
			StaticGUIElement* Input2 = nullptr;
			StaticGUIElement* Input3 = nullptr;
			StaticGUIElement* Output = nullptr;
			GUIButton* BackingButton = nullptr;
			StaticGUIElement* Arrow = nullptr;

			StaticGUIElement* GetInput(const uint8_t i)
			{
				switch (i) {
				case 0:
					return Input1;
				case 1:
					return Input2;
				case 2:
					return Input3;
				default:
					return Input1;
				}

				return Input1;
			};
		};

		struct CraftingArea
		{
			Inventories::ItemSelectionSlot* Input1 = nullptr;
			Inventories::ItemSelectionSlot* Input2 = nullptr;
			Inventories::ItemSelectionSlot* Input3 = nullptr;
			StaticGUIElement* Arrow = nullptr;
			Inventories::ItemSelectionSlot* Output = nullptr;
		};

		static std::array<RecipeDisplay, CRAFTING_DATA_NUM_RECIPES> m_recipeDisplays;
		static CraftingArea m_craftingArea;

		/// <summary>
		/// Adds a recipe autocomplete option for the following items.
		/// </summary>
		/// <param name="inputItem"></param>
		/// <param name="inputItem2"></param>
		/// <param name="inputItem3"></param>
		/// <param name="returnItem"></param>
		/// <param name="index"></param>
		/// <returns></returns>
		static RecipeDisplay addRecipeOption(const ITEM_TYPE& inputItem, const ITEM_TYPE& inputItem2, const ITEM_TYPE& inputItem3, const ITEM_TYPE& returnItem, const int& index);

		/// <summary>
		/// Creates the space to input items to create a new item and find new recipes.
		/// </summary>
		static void addCraftingDisplay();

		static void recipeOptionButtonCallback(int buttonID);

		/// <summary>
		/// Updates the output slot with the correct item.
		/// </summary>
		static void detectOutput();

		/// <summary>
		/// Removes the items from the player and GUI and gives the result.
		/// </summary>
		/// <param name="buttonID"></param>
		static void completeRecipe(int buttonID);

		/// <summary>
		/// Called when the item content of a crafting slot is updated.
		/// </summary>
		/// <param name="buttonID"></param>
		inline static void updateCraftingSlot(int buttonID)
		{
			detectOutput();
		}
	};
}