#pragma once

#include "../../../Objects/Helpers/Interactable.h"
#include "./Trigger.h"

namespace GAME_NAME::Objects::Mechanical
{
	class TriggerLever
		: public Interactable, public Trigger
	{
	public:
		TriggerLever(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, bool leverOn = false, Triggerable* effect = nullptr);

		void Update(GLFWwindow* window) override;

	protected:
		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state);

		bool m_leverOn = false;
	};
}