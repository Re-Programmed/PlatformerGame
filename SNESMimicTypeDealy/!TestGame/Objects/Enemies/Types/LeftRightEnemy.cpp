#include "LeftRightEnemy.h"
#include "../../../../Utils/Time/GameTime.h"

#if _DEBUG
#include "../../../TestGame.h"
#endif

namespace GAME_NAME::Objects::Enemies
{
	LeftRightEnemy::LeftRightEnemy(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, Vec2 leftAnchor, Vec2 rightAnchor, LeftRightEnemyAttributes* attributes, size_t saveID)
		: Enemy(position, scale, sprite, attributes, saveID), m_leftAnchor(leftAnchor), m_rightAnchor(rightAnchor), m_movingLeft(std::rand() < (RAND_MAX/2))
	{

	}

	LeftRightEnemy::~LeftRightEnemy()
	{
	}

#if _DEBUG
	void LeftRightEnemy::Render(const Vec2& cameraPosition)
	{
		if (TestGame::ThePlayer->GetDebug())
		{
			auto const startPoint = Renderer::GetSprite(SpriteBase(53));
			startPoint->Render(cameraPosition, m_leftAnchor + Vec2{ 0, 8 + (m_movingLeft * 5.f) }, {16, 16});

			auto const endPoint = Renderer::GetSprite(SpriteBase(54));
			endPoint->Render(cameraPosition, m_rightAnchor + Vec2{ 0, 8 + ((!m_movingLeft) * 5.f) }, { 16, 16 });
		}

		ActiveBoxCollisionGravityObject::Render(cameraPosition);
	}
#endif

	void LeftRightEnemy::Update(GLFWwindow* window)
	{
		Enemy::Update(window);

		if(completedPathfinding())
		{
			const float& endpointPause = static_cast<LeftRightEnemyAttributes*>(m_enemyAttributes)->EndpointPause;
			if (endpointPause > 0.F)
			{
				m_pauseTimeCounter += (float)Utils::Time::GameTime::GetScaledDeltaTime();

				if (m_pauseTimeCounter < endpointPause)
				{
					return;
				}

				m_pauseTimeCounter = 0.F;
			}

			m_movingLeft = !m_movingLeft;

			Vec2& towards = m_movingLeft ? m_leftAnchor : m_rightAnchor;

			if (towards.Y < 0)
			{
				towards.Y = m_position.Y;
			}

			setPathfinding(towards);
		}
	}
}

