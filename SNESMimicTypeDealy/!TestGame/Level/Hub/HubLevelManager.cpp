#include "HubLevelManager.h"

#include "../../../Objects/Tags/ObjectTagManager.h"
#include "../../../Resources/Save/SaveManager.h"

#include "../../TestGame.h"

#include "../../../Objects/Helpers/Interactable.h"

#include "../../Objects/GUI/GUIItemSelectionBox.h"
#include "../../../Objects/GUI/Menus/GUIMenu.h"

#include "../../InputDisplay/DisplayIconManager.h"

#define HUB_L2_PLAYER_HOUSE_TAG "L2PlayerHouse"
#define HUB_L3_PLAYER_HOUSE_TAG "L3PlayerHouse"

#define HUB_SHOP_SHOP_GUI_ITEMS_TO_COINS "/shop"

namespace GAME_NAME::Level
{
	bool HubLevelManager::m_shopGUIOpen = false;

	using namespace GAME_NAME::Resources;

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
				HubLevelManager::OpenShopGUI();
			}

			Input::DisplayIconManager::ShowKeyInputDisplay(Input::DisplayIconManager::INPUT_DISPLAY_KEY_E, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 9 : 0);
		}
	};


	HubLevelManager::HubLevelManager()
	{
		m_currentHubCharacteristics.Load();

		if (TestGame::INSTANCE->GetCurrentLevelPath().ends_with("_shop"))
		{
			loadShopArea();

			return;
		}

		updateHouseUnlock();
	}

	void HubLevelManager::Update(GLFWwindow* window)
	{

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

	void HubLevelManager::OpenShopGUI()
	{
		if (m_shopGUIOpen) { return; }

		m_shopGUIOpen = true;

		GUI::Menus::GUIMenu::LoadMenu(HUB_SHOP_SHOP_GUI_ITEMS_TO_COINS, nullptr);
		GUI::Menus::GUIMenu::OpenMenu();

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
		GUIItemSelectionBox* itemBox = new GUIItemSelectionBox(Vec2{ 75.f, 50.f }, 60.f, items, 8, new std::function<void(GAME_NAME::Items::ITEM_TYPE, int)>(
			[](GAME_NAME::Items::ITEM_TYPE type, int count) {
				printf("You clicked %s with count %d.\n", ITEMTYPE_GetItemTypeName(type).c_str(), count);
			}
		));
		Renderer::LoadGUIElement(itemBox, 2);
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

