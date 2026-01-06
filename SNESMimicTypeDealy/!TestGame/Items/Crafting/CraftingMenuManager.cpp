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

	CraftingMenuManager::CraftingArea CraftingMenuManager::m_craftingArea;

	GUIScrollArea* CraftingMenuManager::m_recipeDisplayListing = nullptr;

	constexpr int RECIPE_ARROW_SPRITE = SpriteBase(262);
	constexpr float RECIPE_ARROW_HEIGHT_RATIO = 7.f / 13.f;
	constexpr float RECIPE_DISPLAY_WIDTH = 18.f;
	constexpr float RECIPE_PADDING = 4.f;

	void CraftingMenuManager::OpenCraftingMenu()
	{
		if (m_currentManager != nullptr)
		{
			return;
		}

		m_backgroundCover->SetSprite(Renderer::GetSprite(SpriteBase(-1)));
		Renderer::LoadGUIElement(m_backgroundCover);

		int i = -1;
		for (int recipeID = 0; recipeID < CRAFTING_DATA_NUM_RECIPES; recipeID++)
		{
			const Recipe& data = CraftingDataManager::GetRecipe(recipeID);
			if (!data.Unlocked) { continue; }
			i++;

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

			m_recipeDisplays[i] = addRecipeOption(in1, in2, in3, data.Return->GetType(), i);

			if (i == 0)
			{
				m_recipeDisplayListing = new GUIScrollArea(Vec2{ m_recipeDisplays[0].BackingButton->GetPosition().X, 0.f }, Vec2{ m_recipeDisplays[0].BackingButton->GetScale().X, TargetResolutionY }, 0U, 0, (CRAFTING_DATA_NUM_RECIPES - 7) * (RECIPE_PADDING + RECIPE_DISPLAY_WIDTH));
				Renderer::LoadGUIElement(m_recipeDisplayListing);
			}

			m_recipeDisplayListing->AddElement(m_recipeDisplays[i].BackingButton);
			m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Input1);
			if (m_recipeDisplays[i].Input2 != nullptr) { m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Input2); }
			if (m_recipeDisplays[i].Input3 != nullptr) { m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Input3); }
			m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Arrow);
			m_recipeDisplayListing->AddElement(m_recipeDisplays[i].Output);
		}

		addCraftingDisplay();

		//Ensure the tooltip object exists. (TODO: delete tooltip upon closing).
		InventoryTooltip::CreateTooltip();

		//TODO: Disable updating upon closing.
		//Will auto begin updating.
		m_currentManager = new CraftingMenuManager();
	}

	bool CraftingMenuManager::CloseCraftingMenu(int maxRecipeClear)
	{
		if (m_currentManager == nullptr)
		{
			return false;
		}

		delete m_currentManager; m_currentManager = nullptr;

		int i = -1;
		for (uint16_t recipeID = 0; recipeID < CRAFTING_DATA_NUM_RECIPES; recipeID++)
		{
			if (!CraftingDataManager::GetRecipe(recipeID).Unlocked) { continue; }
			i++;

			if (i >= maxRecipeClear) { continue; }

			m_recipeDisplayListing->RemoveElement(m_recipeDisplays[i].BackingButton);
			Renderer::UnloadGUIElement(m_recipeDisplays[i].BackingButton, 2);
			delete m_recipeDisplays[i].BackingButton;
			m_recipeDisplayListing->RemoveElement(m_recipeDisplays[i].Input1);
			Renderer::UnloadGUIElement(m_recipeDisplays[i].Input1, 2);
			delete m_recipeDisplays[i].Input1;
			if(m_recipeDisplays[i].Input2 != nullptr)
			{
				m_recipeDisplayListing->RemoveElement(m_recipeDisplays[i].Input2); 
				Renderer::UnloadGUIElement(m_recipeDisplays[i].Input2, 2);
				delete m_recipeDisplays[i].Input2;
			}
			if (m_recipeDisplays[i].Input3 != nullptr)
			{
				m_recipeDisplayListing->RemoveElement(m_recipeDisplays[i].Input3);
				Renderer::UnloadGUIElement(m_recipeDisplays[i].Input3, 2);
				delete m_recipeDisplays[i].Input3;
			}
			m_recipeDisplayListing->RemoveElement(m_recipeDisplays[i].Output);
			Renderer::UnloadGUIElement(m_recipeDisplays[i].Output, 2);
			delete m_recipeDisplays[i].Output;
			m_recipeDisplayListing->RemoveElement(m_recipeDisplays[i].Arrow);
			Renderer::UnloadGUIElement(m_recipeDisplays[i].Arrow, 2);
			delete m_recipeDisplays[i].Arrow;
		}

		Renderer::UnloadGUIElement(m_recipeDisplayListing);
		delete m_recipeDisplayListing;
		m_recipeDisplayListing = nullptr;

		Renderer::UnloadGUIElement(m_backgroundCover);
		//Don't delete backgroundCover as it is reused.

		ItemSelectionSlot::RemoveItemSelectionMenu();
		
		Renderer::UnloadGUIElement(m_craftingArea.Arrow, 2);
		delete m_craftingArea.Arrow;

		GUIManager::UnregisterButton(m_craftingArea.Input1);
		Renderer::UnloadGUIElement(m_craftingArea.Input1, 2);
		delete m_craftingArea.Input1;
		GUIManager::UnregisterButton(m_craftingArea.Input2);
		Renderer::UnloadGUIElement(m_craftingArea.Input2, 2);
		delete m_craftingArea.Input2;
		GUIManager::UnregisterButton(m_craftingArea.Input3);
		Renderer::UnloadGUIElement(m_craftingArea.Input3, 2);
		delete m_craftingArea.Input3;

		GUIManager::UnregisterButton(m_craftingArea.Output);
		Renderer::UnloadGUIElement(m_craftingArea.Output, 2);
		delete m_craftingArea.Output;

		return true;
	}

	void CraftingMenuManager::Update(GLFWwindow* window)
	{
		const Vec2 mousePos = InputManager::GetMouseScreenPosition();

		int i = -1;
		for (uint16_t recipeID = 0; recipeID < CRAFTING_DATA_NUM_RECIPES; recipeID++)
		{			
			const Recipe& recipe = CraftingDataManager::GetRecipe(recipeID);
			if (!recipe.Unlocked) { continue; }
			i++;

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
				InventoryItem* iCheck = recipe.Return;
				InventoryTooltip::UpdateTooltip(i * CRAFTING_MENU_MANAGER_RECIPE_ELEMENT_COUNT + outputIndex, Inventory::ReturnItem{ iCheck, false });
				return;
			}
		}

		InventoryTooltip::UpdateTooltip(255, Inventory::ReturnItem{ nullptr, true }, true);
	}

	void CraftingMenuManager::AttemptAutofillRecipe(const ITEM_TYPE& in1, const ITEM_TYPE& in2, const ITEM_TYPE& in3)
	{
		if (in1 == ITEM_TYPE::NULL_ITEM)
		{
			m_craftingArea.Input1->SetContainedItem(nullptr);
		}else if (m_craftingArea.Input1 != nullptr)
		{
			int foundSlot = -1; bool wasBackpack;
			Inventory::ReturnItem foundItem = TestGame::ThePlayer->GetItemByType(in1, foundSlot, wasBackpack);

			m_craftingArea.Input1->SetContainedItem(foundItem.ri_Item);
		}

		if (in2 == ITEM_TYPE::NULL_ITEM)
		{
			m_craftingArea.Input2->SetContainedItem(nullptr);
		}
		else if (m_craftingArea.Input2 != nullptr)
		{
			int foundSlot = -1; bool wasBackpack;
			Inventory::ReturnItem foundItem = TestGame::ThePlayer->GetItemByType(in2, foundSlot, wasBackpack);

			m_craftingArea.Input2->SetContainedItem(foundItem.ri_Item);
		}

		if (in3 == ITEM_TYPE::NULL_ITEM)
		{
			m_craftingArea.Input3->SetContainedItem(nullptr);
		}
		else if (m_craftingArea.Input3 != nullptr)
		{
			int foundSlot = -1; bool wasBackpack;
			Inventory::ReturnItem foundItem = TestGame::ThePlayer->GetItemByType(in3, foundSlot, wasBackpack);

			m_craftingArea.Input3->SetContainedItem(foundItem.ri_Item);
		}

		detectOutput();
	}


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

	void CraftingMenuManager::addCraftingDisplay()
	{
		Sprite* sp = Renderer::GetSprite(ITEM_SELECTION_BOX_BG_SPRITE);
		GLuint bgTexture = sp->GetSpriteId();
		delete sp;

		m_craftingArea.Input1 = new ItemSelectionSlot(Vec2{ 125.f, (TargetResolutionY / 2.f) - 10.f }, Vec2{ 20.f }, bgTexture, nullptr, true, new std::function<void(int)>(updateCraftingSlot));
		GUIManager::RegisterButton(m_craftingArea.Input1);
		Renderer::LoadGUIElement(m_craftingArea.Input1, 2);
		m_craftingArea.Input2 = new ItemSelectionSlot(Vec2{ 150.f, (TargetResolutionY / 2.f) - 10.f }, Vec2{ 20.f }, bgTexture, nullptr, true, new std::function<void(int)>(updateCraftingSlot));
		GUIManager::RegisterButton(m_craftingArea.Input2);
		Renderer::LoadGUIElement(m_craftingArea.Input2, 2);
		m_craftingArea.Input3 = new ItemSelectionSlot(Vec2{ 175.f, (TargetResolutionY / 2.f) - 10.f }, Vec2{ 20.f }, bgTexture, nullptr, true, new std::function<void(int)>(updateCraftingSlot));
		GUIManager::RegisterButton(m_craftingArea.Input3);
		Renderer::LoadGUIElement(m_craftingArea.Input3, 2);

		sp = Renderer::GetSprite(RECIPE_ARROW_SPRITE);
		m_craftingArea.Arrow = new StaticGUIElement(Vec2{ 200.f, (TargetResolutionY / 2.f) - (10.f * RECIPE_ARROW_HEIGHT_RATIO) }, Vec2{ 20.f, 20.f * RECIPE_ARROW_HEIGHT_RATIO }, sp->GetSpriteId());
		Renderer::LoadGUIElement(m_craftingArea.Arrow, 2);
		delete sp;

		m_craftingArea.Output = new ItemSelectionSlot(Vec2{ 225.f, (TargetResolutionY / 2.f) - 10.f }, Vec2{ 20.f }, bgTexture, nullptr, false, new std::function<void(int)>(completeRecipe));
		GUIManager::RegisterButton(m_craftingArea.Output);
		Renderer::LoadGUIElement(m_craftingArea.Output, 2);
	}

	void CraftingMenuManager::recipeOptionButtonCallback(int buttonID)
	{
		int i = -1;
		for (uint32_t recipeID = 0; recipeID < CRAFTING_DATA_NUM_RECIPES; recipeID++)
		{
			const Recipe& recipe = CraftingDataManager::GetRecipe(recipeID);
			if (!recipe.Unlocked) { continue; }
			i++;

			if (m_recipeDisplays[i].BackingButton->GetButtonId() == buttonID)
			{
				ITEM_TYPE item1 = ITEM_TYPE::NULL_ITEM;
				ITEM_TYPE item2 = ITEM_TYPE::NULL_ITEM;
				ITEM_TYPE item3 = ITEM_TYPE::NULL_ITEM;

				for (auto [type, amount] : recipe.Inputs)
				{
					if (item1 == ITEM_TYPE::NULL_ITEM)
					{
						item1 = type;
					}else if (item2 == ITEM_TYPE::NULL_ITEM)
					{
						item2 = type;
					}else if (item3 == ITEM_TYPE::NULL_ITEM)
					{
						item3 = type;
					}
				}

				AttemptAutofillRecipe(item1, item2, item3);

				std::cout << "CREATES: " << ITEMTYPE_GetItemTypeName(CraftingDataManager::GetRecipe(recipeID).Return->GetType()) << "\n";

				return;
			}
		}
	}

	void CraftingMenuManager::detectOutput()
	{
		m_craftingArea.Output->SetContainedItem(nullptr);

		for (uint32_t recipeID = 0; recipeID < CRAFTING_DATA_NUM_RECIPES; recipeID++)
		{
			Recipe& recipe = CraftingDataManager::GetRecipe(recipeID);

			ITEM_TYPE item1 = ITEM_TYPE::NULL_ITEM;
			ITEM_TYPE item2 = ITEM_TYPE::NULL_ITEM;
			ITEM_TYPE item3 = ITEM_TYPE::NULL_ITEM;

			for (auto [type, amount] : recipe.Inputs)
			{
				if (item1 == ITEM_TYPE::NULL_ITEM)
				{
					item1 = type;
				}
				else if (item2 == ITEM_TYPE::NULL_ITEM)
				{
					item2 = type;
				}
				else if (item3 == ITEM_TYPE::NULL_ITEM)
				{
					item3 = type;
				}
			}

			if (((item1 == ITEM_TYPE::NULL_ITEM && m_craftingArea.Input1->GetContainedItem() == nullptr) || (m_craftingArea.Input1->GetContainedItem() != nullptr && m_craftingArea.Input1->GetContainedItem()->GetType() == item1)) &&
				((item2 == ITEM_TYPE::NULL_ITEM && m_craftingArea.Input2->GetContainedItem() == nullptr) || (m_craftingArea.Input2->GetContainedItem() != nullptr && m_craftingArea.Input2->GetContainedItem()->GetType() == item2)) &&
				((item3 == ITEM_TYPE::NULL_ITEM && m_craftingArea.Input3->GetContainedItem() == nullptr) || (m_craftingArea.Input3->GetContainedItem() != nullptr && m_craftingArea.Input3->GetContainedItem()->GetType() == item3)))
			{
				if (m_craftingArea.Output->GetContainedItem() != nullptr)
				{
					delete m_craftingArea.Output->GetContainedItem();
				}

				m_craftingArea.Output->SetContainedItem(recipe.Return);

				//Discovered a recipe.
				if (!recipe.Unlocked)
				{
					InventoryItem* i1 = m_craftingArea.Input1->GetContainedItem();
					InventoryItem* i2 = m_craftingArea.Input2->GetContainedItem();
					InventoryItem* i3 = m_craftingArea.Input3->GetContainedItem();
					InventoryItem* out = m_craftingArea.Output->GetContainedItem();

					int validCount = 0;
					for (int i = 0; i < CRAFTING_DATA_NUM_RECIPES; i++)
					{
						if (!CraftingDataManager::GetRecipe(i).Unlocked) { continue; }
						validCount++;
					}

					recipe.Unlocked = true;

					CloseCraftingMenu(validCount);
					OpenCraftingMenu();
					m_craftingArea.Input1->SetContainedItem(i1);
					m_craftingArea.Input2->SetContainedItem(i2);
					m_craftingArea.Input3->SetContainedItem(i3);
					m_craftingArea.Output->SetContainedItem(out);
				}

				return;
			}
		}
	}
	void CraftingMenuManager::completeRecipe(int buttonID)
	{
		//No output, invalid recipe.
		if (m_craftingArea.Output->GetContainedItem() == nullptr) { return; }

		if (m_craftingArea.Input1->GetContainedItem() != nullptr)
		{
			TestGame::ThePlayer->RemoveItemByType(m_craftingArea.Input1->GetContainedItem()->GetType());
			//delete m_craftingArea.Input1->GetContainedItem();
		}

		if (m_craftingArea.Input2->GetContainedItem() != nullptr)
		{
			TestGame::ThePlayer->RemoveItemByType(m_craftingArea.Input2->GetContainedItem()->GetType());
			//delete m_craftingArea.Input2->GetContainedItem();
		}

		if (m_craftingArea.Input3->GetContainedItem() != nullptr)
		{
			TestGame::ThePlayer->RemoveItemByType(m_craftingArea.Input3->GetContainedItem()->GetType());
			//delete m_craftingArea.Input3->GetContainedItem();
		}

		m_craftingArea.Input1->SetContainedItem(nullptr);
		m_craftingArea.Input2->SetContainedItem(nullptr);
		m_craftingArea.Input3->SetContainedItem(nullptr);

		if (TestGame::ThePlayer->GetInventory()->AddItem(m_craftingArea.Output->GetContainedItem()) < 0)
		{
			TestGame::ThePlayer->GetBackpack()->AddItem(m_craftingArea.Output->GetContainedItem());
		}

		m_craftingArea.Output->SetContainedItem(nullptr);

		//Ensure the player must update the selection menu as their inventory has now changed.
		ItemSelectionSlot::RemoveItemSelectionMenu();
	}

}