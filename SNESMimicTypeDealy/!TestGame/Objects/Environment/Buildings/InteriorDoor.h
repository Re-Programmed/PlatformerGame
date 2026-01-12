#pragma once

#include "../../../../Objects/Helpers/Interactable.h"

namespace GAME_NAME::Objects::Environment::Buildings
{
	class InteriorDoor
		: public Interactable
	{
	public:
		InteriorDoor(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::string levelDestination, Vec2 exitPosition, bool loadsObjectsOnly = false, bool useLocalState = false);

	protected:
		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state) override;

		void Update(GLFWwindow* window) override;

	protected:
		bool m_used = false, m_loadsObjectsOnly, m_useLocalState;
		const std::string m_levelDestination;
		const Vec2 m_exitPosition;
	};
}