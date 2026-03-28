#include "PassTriggerObject.h"

#include "../Projectile.h"

namespace GAME_NAME::Objects::Environment
{
	PassTriggerObject::PassTriggerObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::vector<Rendering::Sprite*> triggerAnimationSprites, float animationSpeed)
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, scale.X * 1.5f, position, scale, sprite), m_animator(nullptr)
	{
		if (triggerAnimationSprites.size() <= 0) { return; }

		std::vector<std::shared_ptr<Sprite>> sprites;

		for (Rendering::Sprite* sprite : triggerAnimationSprites)
		{
			sprites.push_back(std::shared_ptr<Sprite>(sprite));
		}

		AnimData data{
			.Sprites = sprites
		};

		std::vector<std::shared_ptr<GAME_NAME::Components::Animation::Animation>> animationList;
		animationList.push_back(std::make_shared<GAME_NAME::Components::Animation::Animation>(data, ANIM_12_SPF));
		m_animator = std::make_unique<AnimatorComponent>(animationList);

		m_animator->SetAllowLooping(false);
		m_animator->SetSpeedMult(animationSpeed);
	}

	void PassTriggerObject::trigger()
	{
		//Spawn butter.
		for (int i = 1; i <= 4; i++)
		{
			Projectile* projectile = new Projectile(m_position + Vec2{ m_scale.X / 2.f, m_scale.Y }, 0, 2.f * i, 3, m_textureFlipped);
			Renderer::InstantiateObject(Renderer::InstantiateGameObject(projectile, true, 2, false));
		}
	}

	void PassTriggerObject::Update(GLFWwindow* window)
	{
		m_animator->Update(window, this);

		Interactable::Update(window);
	}

	void PassTriggerObject::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		if (m_hasTriggered) { return; }

		if (m_animator != nullptr)
		{
			m_animator->SetCurrentAnimation(0);
		}

		trigger();

		m_hasTriggered = true;
	}
}
