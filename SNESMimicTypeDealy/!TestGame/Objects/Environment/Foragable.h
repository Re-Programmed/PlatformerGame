#pragma once

#include "../../../Objects/Helpers/Interactable.h"
#include "../../../Objects/GameObjectState.h"

namespace GAME_NAME::Objects::Environment
{
	/// <summary>
	/// Can be used to drop an item.
	/// The sprite of the next id should be the sprite that appears after foraging.
	/// </summary>
	class Forageable
		: public Interactable, public GameObjectState
	{
	public:
		Forageable(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, Items::InventoryItem* drops = nullptr, double forageLength = 1.0);
		void LoadState() override;
		void SaveState() override;

		void Update(GLFWwindow* window) override;

	protected:
		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state) override;

		void setForaged(bool foraged);

		bool m_foraged = false;
	private:
		Items::InventoryItem* const m_drop;

		const double m_forageLength;

		static double m_forageTimer;
	};
}