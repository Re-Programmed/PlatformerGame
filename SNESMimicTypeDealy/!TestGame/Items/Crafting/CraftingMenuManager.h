#pragma once
#include "../../../Objects/GUI/StaticGUIElement.h"
#include "../../../Objects/GUI/GUIButton.h"
#include "../../../Objects/GUI/GUIScrollArea.h"
#include "../ItemType.h"

#include "./CraftingDataManager.h"

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

		static bool CloseCraftingMenu();

		~CraftingMenuManager()
		{
			Utils::RemoveableUpdateable::~RemoveableUpdateable();
		}

		void Update(GLFWwindow* window);

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

		static std::array<RecipeDisplay, CRAFTING_DATA_NUM_RECIPES> m_recipeDisplays;

		static RecipeDisplay addRecipeOption(const ITEM_TYPE& inputItem, const ITEM_TYPE& inputItem2, const ITEM_TYPE& inputItem3, const ITEM_TYPE& returnItem, const int& index);

		static void recipeOptionButtonCallback(int buttonID);
	};
}