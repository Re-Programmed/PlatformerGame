#include "CraftingMenuManager.h"

#include "../../../Objects/GUI/GUIButton.h"


namespace GAME_NAME::Items::Crafting
{

	StaticGUIElement* const CraftingMenuManager::m_backgroundCover = new StaticGUIElement(Vec2(0, 0), Vec2(TargetResolutionX + 30, TargetResolutionY + 30), 1);

	std::array<std::array<IGUIElement*, CRAFTING_MENU_MANAGER_RECIPE_ELEMENT_COUNT>, CRAFTING_DATA_NUM_RECIPES>  CraftingMenuManager::m_recipeDisplays;

	void CraftingMenuManager::OpenCraftingMenu()
	{
		m_backgroundCover->SetSprite(Renderer::GetSprite(SpriteBase(-1)));
		Renderer::LoadGUIElement(m_backgroundCover);

		for (int i = 0; i < CRAFTING_DATA_NUM_RECIPES; i++)
		{
			const Recipe& data = CraftingDataManager::GetRecipe(i);
			//m_recipeDisplays[i] = addRecipeOption(data.Inputs[0], data.Input2, data.Input3, data.Return, i);
		}
	}

	constexpr int RECIPE_ARROW_SPRITE = 12;
	constexpr float RECIPE_DISPLAY_WIDTH = 18.f;
	constexpr float RECIPE_PADDING = 4.f;

	std::array<IGUIElement*, CRAFTING_MENU_MANAGER_RECIPE_ELEMENT_COUNT> CraftingMenuManager::addRecipeOption(const ITEM_TYPE& inputItem, const ITEM_TYPE& inputItem2, const ITEM_TYPE& inputItem3, const ITEM_TYPE& returnItem, const int& index)
	{
		std::array<IGUIElement*, CRAFTING_MENU_MANAGER_RECIPE_ELEMENT_COUNT> returnData;

#define RecipeOptionElement(posIndex) new StaticGUIElement(backing->GetPosition() + Vec2{ RECIPE_PADDING + itemScale * posIndex, (backing->GetScale().Y - itemScale) / 2.f }, Vec2{ itemScale }, 1)

		GUIButton* backing = new GUIButton(Vec2(10, TargetResolutionY - 10 - (1 + index) * (RECIPE_PADDING + RECIPE_DISPLAY_WIDTH)), Vec2(TargetResolutionX/4, RECIPE_DISPLAY_WIDTH), 1, new std::function<void(int)>(recipeOptionButtonCallback));
		backing->SetSprite(Renderer::GetSprite(SpriteBase(-1)));
		Renderer::LoadGUIElement(backing, 2);

		const float itemScale = (backing->GetScale().X - RECIPE_PADDING * 2.f) / 5.f /*5 elements fit*/;

		//Input 1
		Sprite* sp = ITEMTYPE_GetItemTypeTexture(inputItem);
		StaticGUIElement* input_item = RecipeOptionElement(0);
		input_item->SetSprite(sp);
		Renderer::LoadGUIElement(input_item, 2);
		returnData[0] = input_item;

		//Input 2
		if (inputItem2 != NULL_ITEM)
		{
			sp = ITEMTYPE_GetItemTypeTexture(inputItem);
			StaticGUIElement* input_item2 = RecipeOptionElement(1);
			input_item2->SetSprite(sp);
			Renderer::LoadGUIElement(input_item2, 2);
			returnData[1] = input_item2;
		}

		//Input 3
		if (inputItem2 != NULL_ITEM)
		{
			sp = ITEMTYPE_GetItemTypeTexture(inputItem);
			StaticGUIElement* input_item3 = RecipeOptionElement(2);
			input_item3->SetSprite(sp);
			Renderer::LoadGUIElement(input_item3, 2);
			returnData[2] = input_item3;
		}

		//Recipe Arrow
		sp = Renderer::GetSprite(RECIPE_ARROW_SPRITE);
		StaticGUIElement* input_item4 = RecipeOptionElement(3);
		input_item4->SetSprite(sp);
		Renderer::LoadGUIElement(input_item4, 2);

		//Output
		sp = ITEMTYPE_GetItemTypeTexture(returnItem);
		StaticGUIElement* input_item5 = RecipeOptionElement(4);
		input_item5->SetSprite(sp);
		Renderer::LoadGUIElement(input_item5, 2);
		returnData[3] = input_item5;

		return returnData;
	}

	void CraftingMenuManager::recipeOptionButtonCallback(int buttonId)
	{
		
	}
}