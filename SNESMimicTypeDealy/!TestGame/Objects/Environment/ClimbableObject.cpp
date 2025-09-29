#include "ClimbableObject.h"

namespace GAME_NAME::Objects::Environment
{
	ClimbableObject::ClimbableObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite)
		: Interactable(PLAYER_MOVE_UP, InputManager::KEY_STATE_PRESSED, position - Vec2{ 4.f, 0.f }, scale + Vec2{ 8.f, 0.f }, sprite, 0.f, true)
	{

	}

	void ClimbableObject::Render(const Vec2& cameraPosition)
	{
		m_position.X += 4.f;
		m_scale.X -= 8.f;
		Interactable::Render(cameraPosition);
		m_position.X -= 4.f;
		m_scale.X += 8.f;
	}


	void ClimbableObject::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		if (player->GetClimbing()) { return; }
		player->SetClimbing(this);
	}
}

