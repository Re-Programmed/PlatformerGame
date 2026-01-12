#include "Bench.h"
#include "../../../../Input/InputManager.h"
#include "../../Player/Player.h"

namespace GAME_NAME::Objects::Environment::Buildings
{
	Bench::Bench(Vec2 position, Vec2 scale, Sprite* sprite)
		: Interactable(PLAYER_INTERACT, GAME_NAME::InputManager::KEY_STATE_PRESSED, position, scale, sprite)
	{

	}

	void Bench::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{		
		//Ensure player is "slow enough to engage legs to plop on a bench."
		if (player->GetVelocity().X < 7)
		{
			//Player is not already sitting.
			if (player->GetAnimationState() != Player::Player::SITTING_FORWARD)
			{
				player->SetFrozen(true, Player::Player::SITTING_FORWARD);
				player->SetTextureFlipped(true);
			}
		}
	}
}
