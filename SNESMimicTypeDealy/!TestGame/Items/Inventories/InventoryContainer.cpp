#include "InventoryContainer.h"
#include <cstdarg>
#include "../../../Input/InputManager.h"
#include "../../InputDisplay/DisplayIconManager.h"
#include "InventoryContainerRenderer.h"
#include "../../../Resources/Save/SaveManager.h"
#include "../../../Objects/StateSaver.h"
#include "../Types/Tool.h"

namespace GAME_NAME::Items::Inventories
{

	InventoryContainer::InventoryContainer(std::string name, uint8_t size, Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, float rotation, bool performSave)
		: Inventory(name, size), Interactable(OPEN_INVENTORY_CONTAINER_KEY, InputManager::KEY_STATE_NONE, scale.X, position, scale, sprite, rotation), GameObjectState(saveID), m_performSave(performSave)
	{
		if (m_performSave)
		{
			LoadState();
		}

		this->SetToBeSaved(false);
	}

	InventoryContainer::InventoryContainer(std::string name, uint8_t size, Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, float rotation, int itemCount, ...)
		: Inventory(name, size), Interactable(OPEN_INVENTORY_CONTAINER_KEY, InputManager::KEY_STATE_NONE, scale.X, position, scale, sprite, rotation), GameObjectState(saveID)
	{
		LoadState();

using namespace std;
		va_list args;
		va_start(args, itemCount);

		for (int i = 0; i < itemCount; i++)
		{
			AddItem(va_arg(args, InventoryItem*));
		}

		va_end(args);
	}

	void InventoryContainer::OpenGUI()
	{
		if (m_isOpen) { return; }

		InventoryContainerRenderer::OpenInventoryContainer(this);
		m_isOpen = true;
	}

	void InventoryContainer::CloseGUI()
	{
		if (!m_isOpen) { return; }

		InventoryContainerRenderer::CloseCurrentContainer();
		m_isOpen = false;
	}

	void InventoryContainer::LoadState()
	{
		std::string loadData("");
		Resources::SaveManager::GetLevelString(loadData, m_objectSaveID);

		if (loadData.size() < 3) { return; }

		m_loadedFromState = true;

		std::stringstream ss(loadData);
		std::string line;

		int i = 0;
		while (std::getline(ss, line, '/'))
		{
			if (line.size() < 1) { continue; }
			if (line == "NULL")
			{
				SetItem(i, nullptr);
				i++;
				continue;
			}
			
			//Found an item in saved data, decode it.
			const char& firstChar = line.at(0);

			InventoryItem* item = InventoryItem::DecodeItemString(line);

			SetItem(i, item);
			i++;
		}
	}

	void InventoryContainer::SaveState()
	{
		std::string data("");

		for (int i = 0; i < m_size; i++)
		{
			ReturnItem item = GetItem(i);
			if (!item.ri_IsNull)
			{
				data += item.ri_Item->Encode();
			}
			else {
				data += "NULL";
			}

			data += "/";
		}

		Resources::SaveManager::SaveLevelString(data, m_objectSaveID);
	}

	void InventoryContainer::Update(GLFWwindow* window)
	{
		if (m_isOpen)
		{
			/*if (InputManager::GetKeyUpDown(OPEN_INVENTORY_CONTAINER_KEY) & InputManager::KEY_STATE_PRESSED)
			{
				CloseGUI();
			}*/
			
			//If the inventory is open, update it to detect clicking on slots.
			InventoryContainerRenderer::UpdateCurrentInventoryContainer();

			//The inventory is open so changes may have been made, set to be saved.
			if (!m_toBeSaved && m_performSave)
			{
				StateSaver::RegisterToBeSaved(this);
				SetToBeSaved(true);
			}
		}

		Interactable::Update(window);
	}

	void InventoryContainer::onInteract(std::shared_ptr<GAME_NAME::Objects::Player::Player> player, InputManager::KEY_STATE state)
	{

using namespace GAME_NAME;
		Input::DisplayIconManager::ShowKeyInputDisplay(Input::DisplayIconManager::INPUT_DISPLAY_KEY_E, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)));

		if (state & InputManager::KEY_STATE_PRESSED)
		{
			if (!m_isOpen)
			{
				OpenGUI();
			}
			else {
				CloseGUI();
			}
		}
	}

}
