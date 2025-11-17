#include "FeralRabbit.h"

#include "../../../TestGame.h"
#include "../../../../Utils/Time/GameTime.h"

/*
	--- FERAL RABBIT ATTRIBUTES ---
*/
#define FERAL_RABBIT_AGRESSION_DISTANCE 200
#define FERAL_RABBIT_AWAKEN_DISTANCE 50
#define FERAL_RABBIT_ATTACK_DISTANCE 10

#define FERAL_RABBIT_BASE_ATTACK_DAMAGE 12
#define FERAL_RABBIT_BASE_ATTACK_DELAY 1.25f

#define FERAL_RABBIT_AGRESSION_SPEED 2400.f
#define FERAL_RABBIT_DEFAULT_SPEED 1000.f

namespace GAME_NAME::Objects::Enemies
{
	FeralRabbit::FeralRabbit(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, int jumpingSprite, PassiveRabbitAttributes* attributes, bool beginSleeping, size_t saveID)
		: PassiveRabbit(position, scale, sprite, jumpingSprite, attributes, saveID), m_isAsleep(beginSleeping)
	{
		m_enemyAttributes->MovementSpeed = FERAL_RABBIT_DEFAULT_SPEED;
		m_enemyAttributes->TerminalMovementSpeed = FERAL_RABBIT_DEFAULT_SPEED * 2;
	}

	void FeralRabbit::Update(GLFWwindow* window)
	{
		if (m_isDead) 
		{		
			PassiveRabbit::Update(window);
			return;
		}

		if (m_actionTimer > 0.f)
		{
			m_actionTimer -= Utils::Time::GameTime::GetScaledDeltaTime();
		}

		float distanceToPlayer = Vec2::Distance(m_position, TestGame::ThePlayer->GetPosition() + Vec2{ TestGame::ThePlayer->GetScale().X / 2.f, 0.f });

		if (m_isAsleep)
		{
			//delete m_sprite; //Cannot delete original sprite since it is used by PassiveRabbit as m_baseSprite.
			m_sprite.reset(Renderer::GetSprite(95));
			
			if (distanceToPlayer < FERAL_RABBIT_AWAKEN_DISTANCE)
			{
				m_isAsleep = false;
			}

			return;
		}

		//Player is close, use attacking AI
		if (distanceToPlayer < FERAL_RABBIT_AGRESSION_DISTANCE && m_actionTimer <= 0.f)
		{
			m_enemyAttributes->MovementSpeed = FERAL_RABBIT_AGRESSION_SPEED * (m_supercharge + 1.25f)/1.5f;
			m_allowPathfinding = true;
			setPathfinding(TestGame::ThePlayer->GetPosition());
			updateTextures(window);
			Enemy::Update(window);

			if (distanceToPlayer < FERAL_RABBIT_ATTACK_DISTANCE)
			{
				if (distanceToPlayer < FERAL_RABBIT_ATTACK_DISTANCE / 2.f)
				{
					//Stop moving.
					this->setPathfinding(this->m_position);
				}

				//Damage player since rabbit is in range.
				//TODO: Add slashing animation.
				TestGame::ThePlayer->Damage(FERAL_RABBIT_BASE_ATTACK_DAMAGE * (m_supercharge + 1), this, false);
				m_actionTimer = FERAL_RABBIT_BASE_ATTACK_DELAY;
			}

			return;
		}
		else {
			m_enemyAttributes->MovementSpeed = FERAL_RABBIT_DEFAULT_SPEED;
		}

		PassiveRabbit::Update(window);
	}

}