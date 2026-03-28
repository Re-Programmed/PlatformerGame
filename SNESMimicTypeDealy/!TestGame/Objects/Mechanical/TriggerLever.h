#pragma once

#include "../../../Objects/Helpers/Interactable.h"
#include "./Trigger.h"

#include "../../../Objects/MiscState.h"

namespace GAME_NAME::Objects::Mechanical
{
	class TriggerLever
		: public Interactable, public Trigger, public MiscState
	{
	public:
		TriggerLever(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, bool leverOn = false, Triggerable* effect = nullptr, std::string hoverDescription = "");

		~TriggerLever();

		void Update(GLFWwindow* window) override;

		void Decode(const SaveParam params);
		SaveParam Encode();
	protected:
		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state);

		bool m_leverOn = false;

	private:
		const std::string m_hoverDescription;
	};
}