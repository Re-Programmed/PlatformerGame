#pragma once

#include "../../../Objects/Helpers/Interactable.h"
#include "./Trigger.h"

namespace GAME_NAME::Objects::Mechanical
{
	class TriggerKeyHole
		: public Interactable, public Trigger, public MiscState
	{
	public:
		TriggerKeyHole(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, Triggerable* effect = nullptr, bool keyRemovable = true);

		TriggerKeyHole();

		~TriggerKeyHole();

		void Update(GLFWwindow* window) override;

		void Decode(const SaveParam params);
		SaveParam Encode();

	protected:
		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state);

		bool m_twoKeysIn = false SERIALIZED;
		bool m_keyIn = false SERIALIZED;
		const bool m_keyRemovable;
	};
}