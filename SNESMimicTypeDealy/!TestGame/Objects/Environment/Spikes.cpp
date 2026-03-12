#include "Spikes.h"

#include "../../TestGame.h"
#include "../../../Utils/CollisionDetection.h"

namespace GAME_NAME::Objects::Environment
{
	void Spikes::Update(GLFWwindow* window)
	{
		if (Utils::CollisionDetection::PointWithinBoxBL(TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale() / 2.f, m_position, m_scale))
		{
			const Vec2 playerVelocity = TestGame::ThePlayer->GetVelocity();

			float damageMultiplier = 0.f;

			switch (m_facing)
			{
			case 1:
			case 3:
				damageMultiplier = std::abs(playerVelocity.X) / 1000.f;
				break;
			case 0:
			case 2:
			default:
				damageMultiplier = std::abs(playerVelocity.Y) / 100.f;
				break;
			}

			if (damageMultiplier <= 0.05f) { return; }

			TestGame::ThePlayer->Damage(m_damage * damageMultiplier, this, true, true);
			TestGame::ThePlayer->GetPhysics()->SetVelocity(TestGame::ThePlayer->GetPhysics()->GetVelocity() / 2.f);

			if (!m_hasBeenHit)
			{
				m_hasBeenHit = true;
				SetSpriteId(m_sprite->GetSpriteId() + 1);
			}
		}
	}
}
