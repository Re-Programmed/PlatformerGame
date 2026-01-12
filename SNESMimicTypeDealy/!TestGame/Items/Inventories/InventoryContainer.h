#pragma once

#include "../Inventory.h"
#include "../../../Objects/GameObject.h"
#include "../../../Objects/Helpers/Interactable.h"
#include "../../Objects/Player/Player.h"
#include "../../../Objects/GameObjectState.h"

#define OPEN_INVENTORY_CONTAINER_KEY keyRef::PLAYER_INTERACT

namespace GAME_NAME::Items::Inventories
{
	class InventoryContainer
		: public Inventory, public Interactable, public GameObjectState
	{
	public:
		InventoryContainer(std::string name, uint8_t size, Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, float rotation = 0.f, bool performSave = true);
		//Takes a list of InventoryItem* to use for the inventory of this container.
		InventoryContainer(std::string name, uint8_t size, Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, float rotation, int itemCount, ...);

		void OpenGUI();
		void CloseGUI();

		//Called when a container is created to load its last saved state.
		void LoadState() override;
		//Called when the game is saved to store all the container contents to a file.
		void SaveState() override;

		void Update(GLFWwindow* window) override;

		void onInteract(std::shared_ptr<GAME_NAME::Objects::Player::Player> player, InputManager::KEY_STATE state) override;

		inline bool GetOpen()
		{
			return m_isOpen;
		}

		inline bool GetWasLoadedFromState()
		{
			return m_loadedFromState;
		}
	private:
		bool m_isOpen = false;

		/// <summary>
		/// Is true if this inventory was loaded from a save game instead of being the default inventory.
		/// </summary>
		bool m_loadedFromState = false;

		const bool m_performSave = false;
	};
}