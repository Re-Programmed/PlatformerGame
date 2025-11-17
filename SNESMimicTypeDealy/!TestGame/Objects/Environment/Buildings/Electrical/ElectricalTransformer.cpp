#include "ElectricalTransformer.h"

#include "../../../../TestGame.h"
#include "../../Effects/ElectricalZap.h"
#include "../../Effects/FlashEffect.h"
#include "../../../../../Utils/Time/GameTime.h"

#include "../../../Enemies/Enemy.h"

namespace GAME_NAME::Objects::Environment::Buildings
{

	void ElectricalTransformer::Update(GLFWwindow* window)
	{
		if (m_randomZapTimer > 3.0)
		{
			m_randomZapTimer = 0.0;

			ElectricalZap::CreateZapBetweenPoints(m_position + m_scale / 2.f + Vec2{ 6.f }, m_position + m_scale / 2.f + Vec2{18.f, 0.f});
			ElectricalZap::CreateZapBetweenPoints(m_position + m_scale / 2.f - Vec2{ 6.f }, m_position + m_scale / 2.f - Vec2{ 18.f, 0.f });
		}
		else {
			m_randomZapTimer += Utils::Time::GameTime::GetScaledDeltaTime();
		}

		if (m_cooldown > 0)
		{
			m_cooldown -= Utils::Time::GameTime::GetScaledDeltaTime();
			return;
		}

		std::shared_ptr<Player::Player>& player = TestGame::ThePlayer;

		Vec2 center = this->m_position + m_scale / 2.f;

		if (Vec2::Distance(center, player->GetPosition() + player->GetScale() / 2.f) < m_zapRadius)
		{
			ElectricalZap::CreateZapBetweenPoints(center, player->GetPosition() + player->GetScale() / 2.f + Vec2{0.f, 10.f});
			FlashEffect::CreateFlash(player->GetPosition() + player->GetScale() / 2.f, 30.f, 0.5, Vec3{ 1.0f, 1.0f, 0.66f });
			m_cooldown = ELECTRICAL_TRANSFORMER_ZAP_COOLDOWN;

			//Make damage adjustable.
			player->Damage(7.f, this, false);
		}
		
		for (Enemies::Enemy* enemy : Enemies::Enemy::EnemyRegistry)
		{
			if (Vec2::Distance(center, enemy->GetPosition() + enemy->GetScale() / 2.f) < m_zapRadius + 20.f /*TODO: Make changeable (enemyZapRadius)*/)
			{
				ElectricalZap::CreateZapBetweenPoints(center, enemy->GetPosition() + enemy->GetScale() / 2.f);
				FlashEffect::CreateFlash(enemy->GetPosition() + enemy->GetScale() / 2.f, enemy->GetScale().X + 5.f, 0.5, Vec3{ 0.66f, 0.66f, 1.0f });
				enemy->Supercharge(ELECTRICAL_TRANSFORMER_SUPERCHARGE_AMOUNT);
				m_cooldown = ELECTRICAL_TRANSFORMER_ZAP_COOLDOWN;
			}
		}
	}


}


