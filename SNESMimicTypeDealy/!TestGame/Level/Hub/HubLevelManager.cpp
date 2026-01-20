#include "HubLevelManager.h"

#include "../../../Objects/Tags/ObjectTagManager.h"
#include "../../../Resources/Save/SaveManager.h"

#include "../../TestGame.h"

#include "../../../Objects/Helpers/Interactable.h"

#include "../../../Objects/GUI/Menus/GUIMenu.h"

#include "../../InputDisplay/DisplayIconManager.h"

#include "../../Objects/Player/Currency.h"
#include "HouseManager.h"

#include "DepartManager.h"

#define HUB_L2_PLAYER_HOUSE_TAG "L2PlayerHouse"
#define HUB_L3_PLAYER_HOUSE_TAG "L3PlayerHouse"

#define HUB_SHOP_SHOP_GUI_ITEMS_TO_COINS "/shop"

namespace GAME_NAME::Level
{
	bool HubLevelManager::m_shopGUIOpen = false;

	using namespace GAME_NAME::Resources;

	HubLevelManager::ShopGUI HubLevelManager::m_currentShop = HubLevelManager::ShopGUI{ nullptr, nullptr };

	class FountainInteractable
		: public Interactable
	{
	public:
		FountainInteractable()
			: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, 34.f, Vec2{ 204.f, 132.f }, Vec2{ 1.f, 1.f }, Renderer::GetSprite(1), false)
		{

		}

	protected:
		void onInteract(std::shared_ptr<Objects::Player::Player> player, InputManager::KEY_STATE state) override
		{
			if (state & InputManager::KEY_STATE_PRESSED)
			{
				//TODO: Play coin sound.
				Currency::RemoveCrumbs(1);
			}

			Input::DisplayIconManager::ShowKeyInputDisplay(Input::DisplayIconManager::INPUT_DISPLAY_KEY_E, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 9 : 0);
		}
	};

	FountainInteractable* HubLevelManager_FountainInteractable = nullptr;

	class ShopInteractable
		: public Interactable
	{
	public:
		ShopInteractable()
			: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, 20.f, Vec2{ 120.f, 40.f }, Vec2{ 35.f, 20.f }, Renderer::GetSprite(1), false)
		{
			
		}

		void onInteract(std::shared_ptr<Objects::Player::Player> player, InputManager::KEY_STATE state) override
		{
			if (state & InputManager::KEY_STATE_PRESSED)
			{
				if (!HubLevelManager::OpenShopGUI())
				{
					HubLevelManager::CloseShopGUI();
				}
			}

			Input::DisplayIconManager::ShowKeyInputDisplay(Input::DisplayIconManager::INPUT_DISPLAY_KEY_E, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 9 : 0);
		}
	};


	HubLevelManager::HubLevelManager()
	{
		//--Do for any level--
		m_currentHubCharacteristics.Load();

		Currency::Load();
		Currency::RenderCurrencyDisplay();

		if (TestGame::INSTANCE->GetCurrentLevelPath().ends_with("_shop"))
		{

			//--Do for shop--

			loadShopArea();

			return;
		}
		else if (TestGame::INSTANCE->GetCurrentLevelPath().ends_with("_house"))
		{
			loadHouseArea();

			return;
		}
		else if (TestGame::INSTANCE->GetCurrentLevelPath().ends_with("_depart"))
		{
			DepartManager::Load();

			return;
		}
		else {
			HubLevelManager_FountainInteractable = new FountainInteractable();
			Renderer::InstantiateObject(Renderer::InstantiateGameObject(HubLevelManager_FountainInteractable, false, 1, false));
		}

		//--Do for lobby only--

		updateHouseUnlock();
		HouseManager::CloseHouse();
	}

	/// <summary>
	/// Used to ensure the mouse must be clicked again to perform a second purchase.
	/// </summary>
	bool HubLevelManager_PerformingPurchase = false;

	void HubLevelManager::Update(GLFWwindow* window)
	{
		if (TestGame::INSTANCE->GetCurrentLevelPath().ends_with("_depart"))
		{
			DepartManager::Update();
		}

		if (HubLevelManager_PerformingPurchase && !InputManager::GetMouseButton(0))
		{
			HubLevelManager_PerformingPurchase = false;
		}
	}

	void HubLevelManager::updateHouseUnlock()
	{
		auto [beginl2, endl2] = Tags::ObjectTagManager::GetObjectsWithTag(HUB_L2_PLAYER_HOUSE_TAG);

		for (beginl2; beginl2 != endl2; ++beginl2)
		{
			beginl2->second->SetActive(m_currentHubCharacteristics.PlayerHouseLevel.second >= 2);
		}

		auto [beginl3, endl3] = Tags::ObjectTagManager::GetObjectsWithTag(HUB_L3_PLAYER_HOUSE_TAG);

		for (beginl3; beginl3 != endl3; ++beginl3)
		{
			beginl3->second->SetActive(m_currentHubCharacteristics.PlayerHouseLevel.second >= 3);
		}
	}

	void HubLevelManager::loadShopArea()
	{
		//Create shop.
		Renderer::LoadActiveObject(new ShopInteractable());
	}

	void HubLevelManager::loadHouseArea()
	{
		HouseManager::LoadHouse();
	}

	bool HubLevelManager::OpenShopGUI()
	{
		if (m_shopGUIOpen) { return false; }

		m_shopGUIOpen = true;

		GUI::Menus::GUIMenu::LoadMenu(HUB_SHOP_SHOP_GUI_ITEMS_TO_COINS, nullptr);
		GUI::Menus::GUIMenu::OpenMenu();

		auto shopData = loadShop();

		if (shopData.first.size() < 2 || shopData.second.size() < 2)
		{
			//Randomly generate a new shop because everything is gone...
			generateShop();
		}
		else {
			GUIItemSelectionBox* itemBox = new GUIItemSelectionBox(Vec2{ 75.f, 50.f }, 60.f, shopData.first.begin()._Ptr, shopData.first.size(), nullptr, true);
			Renderer::LoadGUIElement(itemBox, 2);

			GUIItemSelectionBox* itemOutBox = new GUIItemSelectionBox(Vec2{ 175.f, 50.f }, 60.f, shopData.second.begin()._Ptr, shopData.second.size(), new std::function<void(GAME_NAME::Items::ITEM_TYPE, int)>(performPurchase), false);
			Renderer::LoadGUIElement(itemOutBox, 2);

			m_currentShop.ItemIn = itemBox;
			m_currentShop.ItemOut = itemOutBox;
		}

		return true;
	}

	bool HubLevelManager::CloseShopGUI()
	{
		if (!m_shopGUIOpen) { return false; }

		saveShop();

		Renderer::UnloadGUIElement(m_currentShop.ItemIn, 2);
		delete m_currentShop.ItemIn;
		Renderer::UnloadGUIElement(m_currentShop.ItemOut, 2);
		delete m_currentShop.ItemOut;

		GUI::Menus::GUIMenu::RemoveLastMenu();

		m_shopGUIOpen = false;

		return true;
	}



	void HubLevelManager::generateShop()
	{
		//Load item selection area.
		GUIItemSelectionBox::ItemSelection items[8]{
				{ ITEM_TYPE::LOG, 1 },
				{ ITEM_TYPE::APPLE, 12 },
				{ ITEM_TYPE::LOG, 1 },
				{ ITEM_TYPE::APPLE, 12 },
				{ ITEM_TYPE::LOG, 1 },
				{ ITEM_TYPE::APPLE, 12 },
				{ ITEM_TYPE::LOG, 1 },
				{ ITEM_TYPE::APPLE, 12 }
		};
		GUIItemSelectionBox* itemBox = new GUIItemSelectionBox(Vec2{ 75.f, 50.f }, 60.f, items, 8, nullptr, true);
		Renderer::LoadGUIElement(itemBox, 2);




		GUIItemSelectionBox::ItemSelection outItems[8]{
		{ ITEM_TYPE::CRUMBS, 1 },
		{ ITEM_TYPE::CRUMBS, 3 },
		{ ITEM_TYPE::CRUMBS, 3 },
		{ ITEM_TYPE::CRUMBS, 2 },
		{ ITEM_TYPE::CRUMBS, 5 },
		{ ITEM_TYPE::CRUMBS, 4 },
		{ ITEM_TYPE::CRUMBS, 8 },
		{ ITEM_TYPE::CRUMBS, 1 }
		};
		GUIItemSelectionBox* itemOutBox = new GUIItemSelectionBox(Vec2{ 175.f, 50.f }, 60.f, outItems, 8, new std::function<void(GAME_NAME::Items::ITEM_TYPE, int)>(performPurchase), false);
		Renderer::LoadGUIElement(itemOutBox, 2);

		m_currentShop.ItemIn = itemBox;
		m_currentShop.ItemOut = itemOutBox;
	}

	void HubLevelManager::saveShop()
	{
		m_currentShop.Data->ClearItems();

		if (m_currentShop.ItemIn != nullptr)
		{
			for (GUIItemSelectionBox::ItemSelection item : m_currentShop.ItemIn->GetContents())
			{
				m_currentShop.Data->AddItem(item);
			}
		}

		if (m_currentShop.ItemOut != nullptr)
		{
			for (GUIItemSelectionBox::ItemSelection item : m_currentShop.ItemOut->GetContents())
			{
				m_currentShop.Data->AddItem(item);
			}
		}

		Resources::SaveManager::CreateSaveFile(m_currentShop.Data->GetSaveString(), m_currentShop.Data->GetName());
	}

	std::pair<std::vector<GUIItemSelectionBox::ItemSelection>, std::vector<GUIItemSelectionBox::ItemSelection>> HubLevelManager::loadShop()
	{
		std::pair<std::vector<GUIItemSelectionBox::ItemSelection>, std::vector<GUIItemSelectionBox::ItemSelection>> shopData;

		auto savedData = m_currentShop.Data->GetItems();
		for (std::string itemData : *(savedData.get()))
		{
			ShopData::ShopItem currentItem(itemData);

			GUIItemSelectionBox::ItemSelection selection = currentItem.GetSelection();

			if (selection.Type == ITEM_TYPE::CRUMBS)
			{
				shopData.second.push_back(selection);
			}
			else {
				shopData.first.push_back(selection);
			}
		}

		return shopData;
	}

	void HubLevelManager::performPurchase(ITEM_TYPE crumbType, int count)
	{
		if (HubLevelManager_PerformingPurchase)
		{
			return;
		}

		HubLevelManager_PerformingPurchase = true;

		if (m_currentShop.ItemIn != nullptr)
		{
			bool isSelected = false;
			GUIItemSelectionBox::ItemSelection desiredTrade = m_currentShop.ItemIn->GetCurrentSelection(isSelected);

			//No purchase item is selected.
			if (!isSelected) { return; }
	
			//Determine if the player has it.
			int inventoryCount = TestGame::ThePlayer->GetInventory()->Contains(desiredTrade.Type);
			int backpackCount = TestGame::ThePlayer->GetBackpack()->Contains(desiredTrade.Type);

			if (inventoryCount + backpackCount < desiredTrade.Count)
			{
				//TODO: Play nono sound.

				return;
			}
			else {

				//Remove from the player's inventory and backpack.

				int removed = TestGame::ThePlayer->GetInventory()->RemoveItemType(desiredTrade.Type, desiredTrade.Count);
				if (removed < desiredTrade.Count)
				{
					TestGame::ThePlayer->GetBackpack()->Remove(desiredTrade.Type, desiredTrade.Count - removed);
				}
			}

			bool removed = GUIItemSelectionBox::RemoveItemFromSelectionBox(m_currentShop.ItemIn, [desiredTrade](GUIItemSelectionBox::ItemSelection item) {
				return item.Count == desiredTrade.Count && item.Type == desiredTrade.Type;
			}, nullptr);
			
			//The items removal was unsuccessful for some reason.
			if (!removed)
			{
				return;
			}

			//Determine the desired product.
			if (m_currentShop.ItemOut != nullptr)
			{
				bool removed = GUIItemSelectionBox::RemoveItemFromSelectionBox(m_currentShop.ItemOut, [count](GUIItemSelectionBox::ItemSelection item) {
					return item.Count == count;
				}, new std::function<void(GAME_NAME::Items::ITEM_TYPE, int)>(HubLevelManager::performPurchase));

				if (removed)
				{
					//TODO: Remove the item here and give crumbs.
					Currency::AddCrumbs(count);
				}
			}
		}

	}

	void HubLevelManager::HubCharactersitics::Load()
	{
		SaveManager::GetSaveInteger(PlayerHouseLevel.first, PlayerHouseLevel.second);
	}

	void HubLevelManager::HubCharactersitics::Save()
	{
		SaveManager::SaveInteger(PlayerHouseLevel.second, PlayerHouseLevel.first);
	}
}

