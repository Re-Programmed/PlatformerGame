#include "HouseManager.h"

#include "../../TestGame.h"

#include "../../InputDisplay/DisplayIconManager.h"
#include "../../Objects/Player/Currency.h"
#include "../../../Utils/CollisionDetection.h"
#include "../../Objects/Environment/Buildings/InteriorDoor.h"
#include "../../../Objects/StateSaver.h"
#include "../../Objects/StartState.h"


#define HOUSE_MANAGER_FURNITURE_STORE_POSITION { 200, 48 }
#define HOUSE_MANAGER_FURNITURE_STORE_SCALE { 42, 38 }
constexpr int HOUSE_MANAGER_FURNITURE_STORE_SPRITE = 4;

namespace GAME_NAME::Level
{
	std::array<HouseManager::FurnitureTrade, 7> HouseManager::m_furnitureTrades;

	HouseManager::HouseTradeSaveManager* HouseManager::m_houseTradeSaveManager = nullptr;

	std::vector<Furniture*> HouseManager::m_placedFurniture;
	HouseManager* HouseManager::INSTANCE = nullptr;
	HouseManager::HouseInventorySaveManager* HouseManager::INVENTORY_INSTANCE = nullptr;

	HouseManager::FurnitureMenu HouseManager::m_furnitureMenu;

	HouseManager::FurnitureStoreInteractable* HouseManager::m_storeInteractable = nullptr;

	class HouseFrontDoor
		: public Objects::Environment::Buildings::InteriorDoor
	{
	public:
		HouseFrontDoor()
			: Objects::Environment::Buildings::InteriorDoor(Vec2{ 234.f, 0.f }, Vec2{ 34.f, 10.f }, Renderer::GetSprite(20), "/hub", Vec2{ 229.f, 162.f }, false, false)
		{

		}

	protected:
		void onInteract(std::shared_ptr<GAME_NAME::Objects::Player::Player> player, InputManager::KEY_STATE state) override
		{
			//Make sure to save so that the house is all saved.
			if (state & InputManager::KEY_STATE::KEY_STATE_RELEASED)
			{
				if (!m_used)
				{
					//Can't take furniture with you.
					HouseManager::StorePlayerInventory();

					GAME_NAME::Objects::StateSaver::SaveStates();
					GAME_NAME::Objects::StateSaver::SaveMisc();
					GAME_NAME::Objects::StartState::CreateStartState();
				}
			}

			InteriorDoor::onInteract(player, state);
		}
	};

	HouseFrontDoor* HouseManager_HouseFrontDoor;

	void HouseManager::LoadHouse()
	{
		if (INSTANCE != nullptr) { return; }
		INSTANCE = new HouseManager();
		INVENTORY_INSTANCE = new HouseInventorySaveManager();
		m_houseTradeSaveManager = new HouseManager::HouseTradeSaveManager();

		std::shared_ptr<std::vector<std::string>> states = INSTANCE->getStates();

		for (int i = 0; i < states->size(); i++)
		{
			Furniture* furn = new Furniture();
			furn->Decode((*states.get())[i]);
			Renderer::InstantiateObject(Renderer::InstantiateGameObject(furn, true, 1, false));
			PlaceFurniture(furn);
		}

		m_storeInteractable = new FurnitureStoreInteractable(HOUSE_MANAGER_FURNITURE_STORE_POSITION, HOUSE_MANAGER_FURNITURE_STORE_SCALE, Renderer::GetSprite(HOUSE_MANAGER_FURNITURE_STORE_SPRITE));
		Renderer::LoadObject(m_storeInteractable, 1);

		//Load saved shop.
		m_houseTradeSaveManager->LoadFurnitureTrades();
		
		INVENTORY_INSTANCE->Load();

		HouseManager_HouseFrontDoor = new HouseFrontDoor();
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(HouseManager_HouseFrontDoor, false, 2, false));
	}

	void HouseManager::CloseHouse()
	{
		if (INSTANCE == nullptr) { return; }

		Renderer::DestroyObject(m_storeInteractable);
		m_storeInteractable = nullptr;

		delete INSTANCE; INSTANCE = nullptr;
		delete INVENTORY_INSTANCE; INVENTORY_INSTANCE = nullptr;
		delete m_houseTradeSaveManager; m_houseTradeSaveManager = nullptr;
	 
		Renderer::DestroyObject(HouseManager_HouseFrontDoor);
	}

	
	void HouseManager::StorePlayerInventory()
	{
		ScreenInventory* screenInventory = TestGame::ThePlayer->GetInventory();
		storeInventory(screenInventory);
		Backpack* backpack = TestGame::ThePlayer->GetBackpack();
		storeInventory(backpack);
	}

	void HouseManager::OpenFurnitureInventory()
	{
		if (INSTANCE == nullptr) { return; }
		if (m_furnitureMenu.Backing != nullptr) { return; }

		StorePlayerInventory();

		Sprite* sp = Renderer::GetSprite(SpriteBase(-1));
		m_furnitureMenu.Backing = new GUI::StaticGUIElement(Vec2{ 0 }, Vec2{ TargetResolutionX, TargetResolutionY }, sp->GetSpriteId());
		Renderer::LoadGUIElement(m_furnitureMenu.Backing, 1);
		delete sp;

		loadFurnitureSelectionScroll();

		//Furniture list has changed, update states.
		INSTANCE->clearStates();
		for (Furniture* existingFurniture : m_placedFurniture)
		{
			INSTANCE->assignState(existingFurniture);
		}
	}

	bool HouseManager::CloseFurnitureInventory()
	{
		if (INSTANCE == nullptr) { return false; }
		if (m_furnitureMenu.Backing == nullptr) { return false; }

		Renderer::UnloadGUIElement(m_furnitureMenu.Backing, 1);
		delete m_furnitureMenu.Backing; m_furnitureMenu.Backing = nullptr;

		Renderer::UnloadGUIElement(m_furnitureMenu.SelectionMenu, 2);
		delete m_furnitureMenu.SelectionMenu; m_furnitureMenu.SelectionMenu = nullptr;
		Renderer::UnloadGUIElement(m_furnitureMenu.PurchaseMenuItem, 2);
		delete m_furnitureMenu.PurchaseMenuItem; m_furnitureMenu.PurchaseMenuItem = nullptr;
		Renderer::UnloadGUIElement(m_furnitureMenu.PurchaseMenuMoney, 2);
		delete m_furnitureMenu.PurchaseMenuMoney; m_furnitureMenu.PurchaseMenuMoney = nullptr;

		return true;
	}

	bool HouseManager::PlaceFurniture(Furniture* const furniture)
	{
		if (INSTANCE == nullptr) { return false; }

		m_placedFurniture.push_back(furniture);
		INSTANCE->assignState(furniture);
	}

	bool HouseManager::RemoveFurniture(Furniture* const furniture)
	{
		if (INSTANCE == nullptr) { return false; }

		for (int i = 0; i < m_placedFurniture.size(); i++)
		{
			if (m_placedFurniture[i] == furniture)
			{
				m_placedFurniture.erase(m_placedFurniture.begin() + i);
				break;
			}
		}

		//Furniture list has changed, update states.
		INSTANCE->clearStates();
		for (Furniture* existingFurniture : m_placedFurniture)
		{
			INSTANCE->assignState(existingFurniture);
		}
	}

#define HOUSE_MANAGER_FURNITURE_Y_VALID_ADJUSTMENT 3.f

	bool HouseManager::CheckValidPlaceLocation(const Vec2 pos, const Vec2 scale)
	{
		for (Furniture* f : m_placedFurniture)
		{
			Vec2 fScale = f->GetScale();
			Vec2 fPos = f->GetPosition();

			if (fScale.X < 0)
			{
				fScale.X = -fScale.X;
				fPos.X -= fScale.X;
			}

			fScale.Y -= HOUSE_MANAGER_FURNITURE_Y_VALID_ADJUSTMENT;

			if (Utils::CollisionDetection::BoxWithinBox(pos, scale, fPos, fScale))
			{
				return false;
			}
		}

		return true;
	}

	void HouseManager::populateStore()
	{
		std::vector<ITEM_TYPE> allFurniture;
		allFurniture.reserve(10);

		for (int i = 0; i < ITEM_COUNT; i++)
		{
			Items::ITEM_TYPE type = static_cast<Items::ITEM_TYPE>(i);

			if (ITEMTYPE_GetItemData(type).Actions & FURNITURE)
			{
				allFurniture.emplace_back(type);
			}
		}

		for (int i = 0; i < m_furnitureTrades.size(); i++)
		{
			if (m_furnitureTrades[i].Product == Items::NULL_ITEM || m_furnitureTrades[i].Cost == 0)
			{
				int rand = std::rand();
				ITEM_TYPE& type = allFurniture[std::floor(allFurniture.size() * (float)rand / (float)RAND_MAX)];

				bool exists = false;
				for (FurnitureTrade typeCheck : m_furnitureTrades)
				{
					if (typeCheck.Product == type)
					{
						exists = true;
						break;
					}
				}
				if (exists) { continue; }

				m_furnitureTrades[i].Product = type;
				//Value attribute will store how much the furniture should cost.
				m_furnitureTrades[i].Cost = std::stoi(ITEMTYPE_GetItemData(type).Attributes.at(TOOL_ACTION::VALUE));
			}
		}

		//Update states.
		m_houseTradeSaveManager->UpdateSavedFurnitureTrades();
	}

	template<typename T>
	void HouseManager::storeInventory(T* inventory)
	{
		static_assert(std::is_base_of<Inventory, T>::value, "T must inherit from Inventory.");

		for (int i = 0; i < inventory->GetSize(); i++)
		{
			Items::Inventory::ReturnItem item = inventory->GetItem(i);
			if (item.ri_IsNull) { continue; }

			if (ITEMTYPE_GetItemData(item.ri_Item->GetType()).Actions & Items::FURNITURE)
			{
				INVENTORY_INSTANCE->AddItem(item.ri_Item->GetType());
				inventory->SetItem(i, nullptr);
				delete item.ri_Item;
			}
		}
	}

	/// <summary>
	/// Used to ensure that holding the mouse button doesn't continuously select an item.
	/// </summary>
	std::chrono::steady_clock::time_point HouseManager_furnitureInventoryCallback_lastClick = std::chrono::steady_clock::now();
	void HouseManager::furnitureInventoryCallback(Items::ITEM_TYPE type, int count)
	{		
		auto now = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - HouseManager_furnitureInventoryCallback_lastClick).count();

		if (diff < 100)
		{
			HouseManager_furnitureInventoryCallback_lastClick = now;
			return;
		}

		HouseManager_furnitureInventoryCallback_lastClick = now;

		if (INVENTORY_INSTANCE->RemoveItem(type))
		{
			InventoryItem* give = new InventoryItem(type);

			if (TestGame::ThePlayer->GetInventory()->AddItem(give) == -1)
			{
				if (TestGame::ThePlayer->GetBackpack()->AddItem(give) == -1)
				{
					//Add removed item that could not be given to the player back to the storage.
					INVENTORY_INSTANCE->AddItem(type);

					delete give;
					return;
				}
			}

			Renderer::UnloadGUIElement(m_furnitureMenu.SelectionMenu, 2);
			delete m_furnitureMenu.SelectionMenu;
			Renderer::UnloadGUIElement(m_furnitureMenu.PurchaseMenuItem, 2);
			delete m_furnitureMenu.PurchaseMenuItem;
			Renderer::UnloadGUIElement(m_furnitureMenu.PurchaseMenuMoney, 2);
			delete m_furnitureMenu.PurchaseMenuMoney;
			loadFurnitureSelectionScroll();
		}
	}

	void HouseManager::furnitureTradeCallback(Items::ITEM_TYPE type, int count)
	{
		auto now = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - HouseManager_furnitureInventoryCallback_lastClick).count();

		if (diff < 100)
		{
			HouseManager_furnitureInventoryCallback_lastClick = now;
			return;
		}

		HouseManager_furnitureInventoryCallback_lastClick = now;

		if (type != ITEM_TYPE::CRUMBS)
		{
			int cost = std::stoi(ITEMTYPE_GetItemData(type).Attributes.at(TOOL_ACTION::VALUE));

			if (Currency::RemoveCrumbs(cost))
			{
				InventoryItem* item = new InventoryItem(type);

				if (TestGame::ThePlayer->GetInventory()->AddItem(item) == -1)
				{
					if (TestGame::ThePlayer->GetBackpack()->AddItem(item) == -1)
					{
						//Failed to add the item, give player the money back.
						Currency::AddCrumbs(cost);

						
						delete item;
						return;
					}
				}

				//By setting the value of the item to NULL_ITEM, reloading the GUI will repopulate the purchased item as something new.
				for (int i = 0; i < m_furnitureTrades.size(); ++i)
				{
					if (m_furnitureTrades[i].Product == type)
					{
						m_furnitureTrades[i].Product = ITEM_TYPE::NULL_ITEM;
						m_furnitureTrades[i].Cost = 0;
						break;
					}
				}

				CloseFurnitureInventory();
				OpenFurnitureInventory();
			}
		}
	}

	void HouseManager::loadFurnitureSelectionScroll()
	{
		std::vector<GUI::GUIItemSelectionBox::ItemSelection> selection;			selection.reserve(5);
		std::vector<GUI::GUIItemSelectionBox::ItemSelection> selectionPurchase; selectionPurchase.reserve(7);
		std::vector<GUI::GUIItemSelectionBox::ItemSelection> selectionMoney;	selectionMoney.reserve(7);

		for (Items::ITEM_TYPE type : INVENTORY_INSTANCE->GetItems())
		{
			bool foundExisting = false;
			for (int i = 0; i < selection.size(); i++)
			{
				if (selection[i].Type == type)
				{
					selection[i].Count++;
					foundExisting = true;
					break;
				}
			}

			if (foundExisting) { continue; }

			selection.emplace_back(type, 1);
		}

		//Replace any null items with purchaseables.
		populateStore();

		for (FurnitureTrade tradeOption : m_furnitureTrades)
		{
			if (tradeOption.Product == Items::ITEM_TYPE::NULL_ITEM || tradeOption.Cost == 0) { continue; }
			selectionPurchase.emplace_back(tradeOption.Product, 1);
			selectionMoney.emplace_back(ITEM_TYPE::CRUMBS, tradeOption.Cost);
		}

		m_furnitureMenu.SelectionMenu = new GUI::GUIItemSelectionBox(Vec2{ TargetResolutionX / 2.f - 100.f, TargetResolutionY / 2.f - 53.f }, 100.f, selection.size() > 0 ? &selection.front() : nullptr, selection.size(), new std::function<void(Items::ITEM_TYPE, int)>(furnitureInventoryCallback), false);
		Renderer::LoadGUIElement(m_furnitureMenu.SelectionMenu, 2);

		m_furnitureMenu.PurchaseMenuItem = new GUI::GUIItemSelectionBox(Vec2{ TargetResolutionX / 2.f + 75.f, TargetResolutionY / 2.f - 53.f }, 100.f, &selectionPurchase.front(), selectionPurchase.size(), new std::function<void(Items::ITEM_TYPE, int)>(furnitureTradeCallback), false);
		Renderer::LoadGUIElement(m_furnitureMenu.PurchaseMenuItem, 2);

		m_furnitureMenu.PurchaseMenuMoney = new GUI::GUIItemSelectionBox(Vec2{ TargetResolutionX / 2.f + 25.f, TargetResolutionY / 2.f - 53.f }, 100.f, &selectionMoney.front(), selectionMoney.size(), new std::function<void(Items::ITEM_TYPE, int)>(furnitureTradeCallback), false);
		Renderer::LoadGUIElement(m_furnitureMenu.PurchaseMenuMoney, 2);
	}

	MiscState::SaveParam HouseManager::HouseInventorySaveManager::SavedItemType::Encode()
	{
		return std::to_string(Type);
	}

	void HouseManager::HouseInventorySaveManager::SavedItemType::Decode(const SaveParam param)
	{
		this->Type = static_cast<Items::ITEM_TYPE>(std::stoi(param));
	}

	void HouseManager::FurnitureStoreInteractable::onInteract(std::shared_ptr<GAME_NAME::Objects::Player::Player> player, InputManager::KEY_STATE state)
	{
		if (state & InputManager::KEY_STATE_PRESSED)
		{
			if (!HouseManager::CloseFurnitureInventory())
			{
				HouseManager::OpenFurnitureInventory();
			}
		}

		Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 9 : 0);
	}
}

