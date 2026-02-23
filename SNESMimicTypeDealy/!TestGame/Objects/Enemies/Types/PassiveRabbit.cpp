#include "PassiveRabbit.h"

#include "../../../../Utils/Time/GameTime.h"

#include "../../../../Rendering/Renderers/Renderer.h"
#include "FeralRabbit.h"

namespace GAME_NAME::Objects::Enemies
{
	PassiveRabbit::PassiveRabbit(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, int jumpingSprite, PassiveRabbitAttributes* attributes, size_t saveID)
		: Enemy(position, scale, sprite, attributes, saveID), m_baseSprite(sprite->GetSpriteId()), m_jumpingSprite(jumpingSprite)
	{
		m_allowPathfinding = false;
		
		m_physics->SetFrictionDrag(0.f);

		attributes->IgnoreYPathfind = true;
		attributes->TerminalMovementSpeed = 200.f;
	}

	PassiveRabbit::~PassiveRabbit()
	{
		Enemy::~Enemy();
	}


	void PassiveRabbit::Update(GLFWwindow* window)
	{

		//If the passive rabbit becomes supercharged, it will become angry.
		if (m_supercharge > 0)
		{
			Renderer::InstantiateObject(Renderer::InstantiateGameObject(new FeralRabbit(this->m_position, this->m_scale, this->m_sprite.get(), this->m_jumpingSprite), true, 2, false));
			Renderer::DestroyActiveObject(this);
			RemoveSupercharge();
			m_isDead = true;
			return;
		}

		if (m_isDead) { 
			Enemy::Update(window); 
			return; 
		}

		//If the rabbit is currently pathfinding, ignore updating anything until it reaches its destination, then begin countdown.
		if (m_allowPathfinding)
		{
			if (completedPathfinding())
			{
				m_allowPathfinding = false;
			}

			Enemy::Update(window);
		//Rabbit needs a new action.
		} else if (m_actionTimer < 0.0)
		{
			m_actionTimer = 1.2 + ((double)std::rand() / RAND_MAX) * 3.0;
			//Add random direction.
			bool moveDirection = std::rand() < RAND_MAX / 2;

			if ((double)std::rand() < (double)RAND_MAX / 1.5)
			{
				//**Choose to run.

				m_allowPathfinding = true;
				setPathfinding({ m_position.X + ((moveDirection ? 1 : -1) * (50 + (std::rand() * 400 / RAND_MAX))), m_position.Y });

				return;
			}

			//**Choose to jump.


			//Add randomness to jumps.
			float xRand = ((float)std::rand() * 46.f / RAND_MAX) - 23.f;

			if (m_onGround)
			{
				m_physics->AddVelocity({ moveDirection ? (-37.f - xRand) : (37.f - xRand), (((PassiveRabbitAttributes*)(m_enemyAttributes))->JumpHeight * 5.f) });
			}
		}
		//Rabbit is jumping or waiting right now, update timer.
		else {
			m_actionTimer -= Utils::Time::GameTime::GetScaledDeltaTime();
		}

		updateTextures(window);

		Enemy::Update(window);
	}


	void PassiveRabbit::onCollision(Vec2 push, GameObject* self, GameObject* other)
	{
		Enemy::onCollision(push, self, other);

		m_physics->SetVelocity({ 0.f, 0.f });
	}

	void PassiveRabbit::updateTextures(GLFWwindow* window)
	{
		/*
			--TEXTURE UPDATES--
		*/
#pragma region TextureUpdates
		//Flip texture if moving backwards (and not already flipped).
		if ((m_physics->GetVelocity().X > 0.f && m_scale.X > 0) || (m_physics->GetVelocity().X < 0.f && m_scale.X < 0))
		{
			m_scale.X = -m_scale.X;

			//Update position since scale flip is along the side edge -- not middle.
			m_position.X -= m_scale.X;
		}

		//The rabbit is running
		if (m_allowPathfinding)
		{
			int inc8Frame = (int)m_position.X;
			if (inc8Frame % 8 == 0 && inc8Frame != m_lastOddFrameX)
			{
				m_lastOddFrameX = inc8Frame;
				m_oddFrame = !m_oddFrame;

				//Alternate between the jumping sprite and 1 after the jumping sprite.
				SetSprite(Renderer::GetSprite(m_jumpingSprite + m_oddFrame));
			}
			//The rabbit is jumping.
		}
		else if (m_physics->GetVelocity().Y > 3.f || m_physics->GetVelocity().Y < -3.f)
		{
			if (m_sprite->GetSpriteId() == m_baseSprite)
			{
				SetSprite(Rendering::Renderer::GetSprite(m_jumpingSprite));
			}
		}
		else if (m_sprite->GetSpriteId() != m_baseSprite)
		{
			SetSprite(new Sprite(m_baseSprite));
		}
#pragma endregion
	}



}
