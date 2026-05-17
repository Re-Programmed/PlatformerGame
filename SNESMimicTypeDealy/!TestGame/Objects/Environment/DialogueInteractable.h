#pragma once

#include "../../Cutscenes/DialogueManager.h"
#include "../../../Objects/Helpers/Interactable.h"

namespace GAME_NAME::Objects::Environment
{
	class DialogueInteractable
		: public Interactable
	{
	public:
		DialogueInteractable(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::string dialogueSource, std::string setActiveKey = "");

		void Update(GLFWwindow* window) override;
	protected:
		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state) override;
	private:
		const std::string m_dialogueSource;

		bool m_wasInteracted = false;

		/// <summary>
		/// Will set this object active after interaction.
		/// </summary>
		std::string m_setActiveKey;
	};
}