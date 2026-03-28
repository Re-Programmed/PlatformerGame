#pragma once

#include "../../../Objects/Helpers/Interactable.h"

#include "../../../Components/Animation/AnimatorComponent.h"

namespace GAME_NAME::Objects::Environment
{
	class PassTriggerObject
		: public Interactable
	{
	public:
		PassTriggerObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::vector<Rendering::Sprite*> triggerAnimationSprites = std::vector<Rendering::Sprite*>(), float animationSpeed = 1.f);


	protected:
		virtual void trigger();

		void Update(GLFWwindow* window) override;

		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state) override;

		bool m_hasTriggered = false;

	private:
		std::unique_ptr<AnimatorComponent> m_animator;
	};
}