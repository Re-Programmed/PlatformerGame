#include "CraftingMenuManager.h"

#include "../../../Objects/GUI/GUIManager.h"
#include "../Inventories/InventoryTooltip.h"
#include "../../../Utils/CollisionDetection.h"

namespace GAME_NAME::Items::Crafting
{

using namespace Inventories;

	StaticGUIElement* const CraftingMenuManager::m_backgroundCover = new StaticGUIElement(Vec2(0, 0), Vec2(TargetResolutionX + 30, TargetResolutionY + 30), 1);

	std::array<CraftingMenuManager::RecipeDisplay, CRAFTING_DATA_NUM_RECIPES>  CraftingMenuManager::m_recipeDisplays;

	CraftingMenuManager* CraftingMenuManager::m_currentManager = nullptr;

	GUIScrollArea* CraftingMenuManager::m_recipeDisplayListing = nullptr;

	void CraftingMenuManager::OpenCraftingMenu()
	{
		m_backgroundCover->SetSprite(Renderer::GetSprite(SpriteBase(-1)));
		Renderer::LoadGUIElement(m_backgroundCover);


		for (int i = 0; i < CRAFTING_DATA_NUM_RECIPES; i++)
		{
			const Recipe& data = CraftingDataManager::GetRecipe(i);

			uint8_t iIn = 0;
			ITEM_TYPE in1 = ITEM_TYPE::NULL_ITEM, in2 = ITEM_TYPE::NULL_ITEM, in3 = ITEM_TYPE::NULL_ITEM;
			for (auto[ingredient, amount] : data.Inputs)
			{
				switch (iIn++)
				{
				case 0:
					in1 = ingredient;
					break;
				case 1:
					in2 = ingredient;
					break;
				case 2:
					in3 = ingredient;
					break;
				default:
					continue;
				}
			}

			m_recipeDisplays[i] = addRecipeOption(in1, in2, in3, data.Return, i);

			if (i == 0)
			{
				m_recipeDisplayListing = new GUIScrollArea(m_recipeDisplays[0].BackingButton->GetPosition(), Vec2{ m_recipeDisplays[0].BackingButton->GetScale().X, m_recipeDisplays[0].BackingButton->GetScale().Y * 10.f }, 0U, 0.0, 200.0);
				Renderer::LoadGUIElement(m_recipeDisplayListing);
			}

			m_recipeDisplayListing->AddElement(m_recipeDisplays[i].BackingButton);
			m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Input1);
			if (m_recipeDisplays[i].Input2 != nullptr) { m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Input2); }
			if (m_recipeDisplays[i].Input3 != nullptr) { m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Input3); }
			m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Arrow);
			m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Output);
		}

		//Ensure the tooltip object exists. (TODO: delete tooltip upon closing).
		InventoryTooltip::CreateTooltip();

		//TODO: Disable updating upon closing.
		//Will auto begin updating.
		m_currentManager = new CraftingMenuManager();
	}

	bool CraftingMenuManager::CloseCraftingMenu()
	{
		if (m_currentManager == nullptr)
		{
			return false;
		}

		delete m_currentManager;

		for (uint16_t recipeID = 0; recipeID < CRAFTING_DATA_NUM_RECIPES; recipeID++)
		{
			m_recipeDisplayListing->RemoveElement(m_recipeDisplays[recipeID].BackingButton);
			Renderer::UnloadGUIElement(m_recipeDisplays[recipeID].BackingButton, 2);
			delete m_recipeDisplays[recipeID].BackingButton;
			m_recipeDisplayListing->RemoveElement(m_recipeDisplays[recipeID].Input1);
			Renderer::UnloadGUIElement(m_recipeDisplays[recipeID].Input1, 2);
			delete m_recipeDisplays[recipeID].Input1;
			if(m_recipeDisplays[recipeID].Input2 != nullptr)
			{
				m_recipeDisplayListing->RemoveElement(m_recipeDisplays[recipeID].Input2); 
				Renderer::UnloadGUIElement(m_recipeDisplays[recipeID].Input2, 2);
				delete m_recipeDisplays[recipeID].Input2;
			}
			if (m_recipeDisplays[recipeID].Input3 != nullptr)
			{
				m_recipeDisplayListing->RemoveElement(m_recipeDisplays[recipeID].Input3);
				Renderer::UnloadGUIElement(m_recipeDisplays[recipeID].Input3, 2);
				delete m_recipeDisplays[recipeID].Input3;
			}
			m_recipeDisplayListing->RemoveElement(m_recipeDisplays[recipeID].Output);
			Renderer::UnloadGUIElement(m_recipeDisplays[recipeID].Output, 2);
			delete m_recipeDisplays[recipeID].Output;
			m_recipeDisplayListing->RemoveElement(m_recipeDisplays[recipeID].Arrow);
			Renderer::UnloadGUIElement(m_recipeDisplays[recipeID].Arrow, 2);
			delete m_recipeDisplays[recipeID].Arrow;
		}

		Renderer::UnloadGUIElement(m_recipeDisplayListing);
		delete m_recipeDisplayListing;
		m_recipeDisplayListing = nullptr;

		Renderer::UnloadGUIElement(m_backgroundCover);
		//Don't delete backgroundCover as it is reused.

		return true;
	}

	void CraftingMenuManager::Update(GLFWwindow* window)
	{
		for (uint16_t i = 0; i < CRAFTING_DATA_NUM_RECIPES; i++)
		{
			Vec2 mousePos = InputManager::GetMouseScreenPosition();
			
			const Recipe& recipe = CraftingDataManager::GetRecipe(i);

			uint8_t iIn = 0;
			for (auto[inputType, inputCount] : recipe.Inputs)
			{
				if (Utils::CollisionDetection::PointWithinBoxBL(mousePos, m_recipeDisplays[i].GetInput(iIn)->GetPosition(), m_recipeDisplays[i].GetInput(iIn)->GetScale()))
				{
					InventoryItem iCheck = InventoryItem(inputType);
					InventoryTooltip::UpdateTooltip(i * CRAFTING_MENU_MANAGER_RECIPE_ELEMENT_COUNT + iIn, Inventory::ReturnItem{ &iCheck, false });
					return;
				}

				iIn++;
			}

			constexpr uint8_t outputIndex = 3;
			if (Utils::CollisionDetection::PointWithinBoxBL(mousePos, m_recipeDisplays[i].Output->GetPosition(), m_recipeDisplays[i].Output->GetScale()))
			{
				InventoryItem iCheck = InventoryItem(recipe.Return);
				InventoryTooltip::UpdateTooltip(i * CRAFTING_MENU_MANAGER_RECIPE_ELEMENT_COUNT + outputIndex, Inventory::ReturnItem{ &iCheck, false });
				return;
			}
		}

		InventoryTooltip::UpdateTooltip(255, Inventory::ReturnItem{ nullptr, true }, true);
	}

	constexpr int RECIPE_ARROW_SPRITE = SpriteBase(256);
	constexpr float RECIPE_ARROW_HEIGHT_RATIO = 7.f / 13.f;
	constexpr float RECIPE_DISPLAY_WIDTH = 18.f;
	constexpr float RECIPE_PADDING = 4.f;
	
	inline StaticGUIElement* CraftingMenuManager_RecipeOptionElement(const int& posIndex, StaticGUIElement* const backing, const float& itemScale, const GLuint& texture)
	{
		return new StaticGUIElement(backing->GetPosition() + Vec2{ RECIPE_PADDING + itemScale * posIndex, (backing->GetScale().Y - itemScale) / 2.f }, Vec2{ itemScale }, texture);
	}

	inline StaticGUIElement* CraftingMenuManager_RecipeOptionElement(const int& posIndex, StaticGUIElement* const backing, const Vec2& itemScale, const GLuint& texture)
	{
		return new StaticGUIElement(backing->GetPosition() + Vec2{ RECIPE_PADDING + itemScale.X * posIndex, (backing->GetScale().Y - itemScale.Y) / 2.f }, itemScale, texture);
	}
	
	CraftingMenuManager::RecipeDisplay CraftingMenuManager::addRecipeOption(const ITEM_TYPE& inputItem, const ITEM_TYPE& inputItem2, const ITEM_TYPE& inputItem3, const ITEM_TYPE& returnItem, const int& index)
	{
		RecipeDisplay returnData;

		GUIButton* backing = new GUIButton(Vec2(10, TargetResolutionY - 10 - (1 + index) * (RECIPE_PADDING + RECIPE_DISPLAY_WIDTH)), Vec2(TargetResolutionX/4, RECIPE_DISPLAY_WIDTH), 1, new std::function<void(int)>(recipeOptionButtonCallback));
		backing->SetSprite(Renderer::GetSprite(SpriteBase(-1)));
		Renderer::LoadGUIElement(backing, 2);
		GUI::GUIManager::RegisterButton(backing);
		returnData.BackingButton = backing;

		const float itemScale = (backing->GetScale().X - RECIPE_PADDING * 2.f) / 5.f /*5 elements fit*/;

		//Input 1
		Sprite* sp = ITEMTYPE_GetItemTypeTexture(inputItem);
		StaticGUIElement* input_item = CraftingMenuManager_RecipeOptionElement(0, backing, itemScale, sp->GetSpriteId());
		delete sp;
		Renderer::LoadGUIElement(input_item, 2);
		returnData.Input1 = input_item;

		//Input 2
		if (inputItem2 != NULL_ITEM)
		{
			sp = ITEMTYPE_GetItemTypeTexture(inputItem2);
			StaticGUIElement* input_item2 = CraftingMenuManager_RecipeOptionElement(1, backing, itemScale, sp->GetSpriteId());
			delete sp;
			Renderer::LoadGUIElement(input_item2, 2);
			returnData.Input2 = input_item2;
		}

		//Input 3
		if (inputItem3 != NULL_ITEM)
		{
			sp = ITEMTYPE_GetItemTypeTexture(inputItem3);
			StaticGUIElement* input_item3 = CraftingMenuManager_RecipeOptionElement(2, backing, itemScale, sp->GetSpriteId());
			delete sp;
			Renderer::LoadGUIElement(input_item3, 2);
			returnData.Input3 = input_item3;
		}

		//Recipe Arrow
		sp = Renderer::GetSprite(RECIPE_ARROW_SPRITE);
		StaticGUIElement* arrow = CraftingMenuManager_RecipeOptionElement(3, backing, Vec2{ itemScale, itemScale * RECIPE_ARROW_HEIGHT_RATIO }, sp->GetSpriteId());
		delete sp;
		Renderer::LoadGUIElement(arrow, 2);
		returnData.Arrow = arrow;

		//Output
		sp = ITEMTYPE_GetItemTypeTexture(returnItem);
		StaticGUIElement* output_item = CraftingMenuManager_RecipeOptionElement(4, backing, itemScale, sp->GetSpriteId());
		delete sp;
		Renderer::LoadGUIElement(output_item, 2);
		returnData.Output = output_item;

		return returnData;
	}

	void CraftingMenuManager::recipeOptionButtonCallback(int buttonID)
	{
		std::cout << "CRAFT_" << buttonID << "\n";

		for (uint32_t recipeID = 0; recipeID < CRAFTING_DATA_NUM_RECIPES; recipeID++)
		{
			if (m_recipeDisplays[recipeID].BackingButton->GetButtonId() == buttonID)
			{
				std::cout << "CREATES: " << ITEMTYPE_GetItemTypeName(CraftingDataManager::GetRecipe(recipeID).Return) << "\n";

				return;
			}
		}
	}
}