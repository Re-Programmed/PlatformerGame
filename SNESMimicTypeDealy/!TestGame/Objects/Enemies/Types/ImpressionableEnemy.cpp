#include "ImpressionableEnemy.h"

#include "../../../../Utils/Time/GameTime.h"

#include "../../../TestGame.h"

#include "../../../../Audio/SoundEvents.h"

namespace GAME_NAME::Objects::Enemies
{
	using namespace Components::Animation;

	struct ImpressionableAttackType {
		Vec2 Scale;
		std::vector<int> WalkAnimation;
		std::vector<int> AttackAnimation;
		int IdleSprite, UsingPhoneSprite;

		float MovementSpeed;
		float TerminalMovementSpeed;
	};

	const ImpressionableAttackType IMPRESSIONABLE_ATTACK_TYPES[3] = {
		{ Vec2{ 34.f/2.f, 44.f/2.f }, { 159, 160, 159, 161 }, { 162, 163 }, 157, 0, 500.f, 2750.f },
		{ Vec2{ 16.f, 16.f }, { 0 }, { 0 }, 0, 0, 5.f, 20.f },
		{ Vec2{ 16.f, 16.f }, { 0 }, { 0 }, 0, 0, 5.f, 20.f }
	};

	ImpressionableEnemy::ImpressionableEnemy(Vec2 position, AttackType type, bool initiallyAngry, size_t saveId)
		: Enemy(position, IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(type)].Scale, nullptr, new EnemyAttributes(IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(type)].MovementSpeed, IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(type)].TerminalMovementSpeed, new EnemyAttributes::TempAttributes()), saveId),
		m_home(position), m_actionTimer(0.0), m_attackType(type), m_heldItem(ITEM_TYPE::NULL_ITEM)
	{
		AnimData walk_anim_data;
		AnimData attack_anim_data;

		for (int spriteIndex : IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(type)].WalkAnimation)
		{
			walk_anim_data.Sprites.push_back(std::make_shared<Sprite>(Renderer::GetSpriteIdFromTextureId(spriteIndex)));
		}

		for (int spriteIndex : IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(type)].AttackAnimation)
		{
			attack_anim_data.Sprites.push_back(std::make_shared<Sprite>(Renderer::GetSpriteIdFromTextureId(spriteIndex)));
		}

		std::vector<std::shared_ptr<Components::Animation::Animation>> animations;
		animations.push_back(std::make_shared<Components::Animation::Animation>(walk_anim_data, ANIM_12_SPF));
		animations.push_back(std::make_shared<Components::Animation::Animation>(attack_anim_data, ANIM_6_SPF));

		m_allowPathfinding = true;

		m_animator = new AnimatorComponent(animations);
	}

	void ImpressionableEnemy::Update(GLFWwindow* window)
	{
		Enemy::Update(window);
		m_animator->Update(window, this);

		updateMotion();
		updateTextures();
	}


#define IMPRESSIONABLE_ENEMY_STAB_DISTANCE 42.5f
#define IMPRESSIONABLE_ENEMY_ATTACK_INTERVAL 1.0

#define IMPRESSIONABLE_ENEMY_STAB_DAMAGE 7

	void ImpressionableEnemy::Render(const Vec2& cameraPos)
	{
		if (m_attackCharge > 0.0)
		{
			Vec4 singleVert{ 1.f, 1.f - static_cast<float>(m_attackCharge / IMPRESSIONABLE_ENEMY_ATTACK_INTERVAL), 1.f - static_cast<float>(m_attackCharge / IMPRESSIONABLE_ENEMY_ATTACK_INTERVAL), 1.f };
			Vec4 textureColor[4] = {
				singleVert, singleVert, singleVert, singleVert
			};

			DynamicSprite sprite(m_sprite->GetSpriteId());
			sprite.UpdateTextureColor(textureColor);
			sprite.Render(cameraPos, Vec2{ m_position.X + m_scale.X, m_position.Y + m_scale.Y }, Vec2{ -m_scale.X, -m_scale.Y }, m_rotation);
		}
		else {
			Enemy::Render(cameraPos);
		}


		if (m_heldItem != ITEM_TYPE::NULL_ITEM)
		{
			Sprite* heldItemSprite = ITEMTYPE_GetItemTypeTexture(m_heldItem);
			float xOffset = m_scale.X / 2.f + ((m_animator->GetCurrentAnimationIndex() >= 0 && m_animator->GetCurrentAnimation()->GetFrame() % 2 == 0) ? -1.f : 0.f);
			float yOffset = 3.5f;

			if (m_animator->GetCurrentAnimationIndex() == 1 /*Attack animation playing*/)
			{
				yOffset += 2.5f;
				xOffset += 2.5f;
			}

			heldItemSprite->Render(cameraPos, m_position + Vec2{ xOffset, yOffset }, Vec2{8.f * (m_scale.X < 0.f ? -1.f : 1.f), 8.f});
			delete heldItemSprite;
		}

	}


	void ImpressionableEnemy::updateMotion()
	{
		//Angry if player near.
		if (!m_isAngered && Vec2::Distance(TestGame::ThePlayer->GetPosition(), m_position) < 45.f)
		{
			m_isAngered = true;
		}

		//If close enough and a melee attack, start charging attack.
		if (m_attackType == AttackType::Stabber)
		{
			if (m_actionTimer <= 0.0 && m_attackCharge <= 0.0 && Vec2::Distance(TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale()/2.f, m_position + m_scale / 2.f) < IMPRESSIONABLE_ENEMY_STAB_DISTANCE)
			{
				m_attackCharge += 0.1;
			}
		}

		double dt = Utils::Time::GameTime::DeltaTime::GetDeltaTime();

		//If idling, the action timer indicates how long until the next random movement.
		if (!m_isAngered)
		{
			m_actionTimer += dt;
		}
		//Otherwise, action timer is how long the attack animation is taking.
		else if (m_actionTimer > 0.0)
		{
			m_actionTimer -= dt;
		}
		//Otherwise, the enemy is angered but not currently attacking.
		else {
			setPathfinding(TestGame::ThePlayer->GetPosition());
		}

		if (m_attackCharge > 0.0 && m_actionTimer <= 0.0)
		{
			m_attackCharge += dt;

			if (m_attackCharge > IMPRESSIONABLE_ENEMY_ATTACK_INTERVAL)
			{
				m_attackCharge = 0.0;

				//Perform attack.
				attack();
			}
		}

		if (m_actionTimer > 2.5)
		{
			double rand = (double)std::rand() / ((double)RAND_MAX);

			if (rand < 0.025)
			{
				if (!m_isAngered)
				{
					//Too far from spawn point, go back.
					Vec2 randomMotionDestination = m_position + Vec2{ rand < 0.0125 ? -40.f : 40.f, 0.f };
					if (Vec2::Distance(randomMotionDestination, m_home) > 63.f)
					{
						randomMotionDestination = m_home;
					}

					setPathfinding(randomMotionDestination);
					m_actionTimer = 0.0;
				}
			}
		}
	}

	void ImpressionableEnemy::updateTextures()
	{
		float animMomentum = std::abs(m_physics->GetVelocity().X);
		if (m_isAngered && m_actionTimer > 0.0)
		{
			m_animator->SetCurrentAnimation(1);	//Attack animation.
			m_animator->SetSpeedMult(1.0);
		}else if (animMomentum > 0.025f)
		{
			m_animator->SetCurrentAnimation(0); //Walk animation.
			m_animator->SetSpeedMult((double)(animMomentum / 100.f));

			//Flip texture if moving backwards (and not already flipped).
			if ((m_physics->GetVelocity().X < 0.f && m_scale.X < 0) || (m_physics->GetVelocity().X > 0.f && m_scale.X > 0))
			{
				m_scale.X = -m_scale.X;

				//Update position since scale flip is along the side edge -- not middle.
				m_position.X -= m_scale.X;
			}
		}
		else {
			m_animator->SetCurrentAnimation(-1);
			if (m_isAngered)
			{
				m_sprite.reset(Renderer::GetSprite(IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(m_attackType)].IdleSprite + 1));
			}
			else {
				m_sprite.reset(Renderer::GetSprite(IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(m_attackType)].IdleSprite));
			}
		}
	}

	void ImpressionableEnemy::attack()
	{
		m_actionTimer = 1.0;

		GAME_NAME::Audio::SoundEvents::PlaySoundAtPoint(Audio::SoundEvents::Event::ATTACK_SWIPE, m_position, 0.75f);

		float damage = IMPRESSIONABLE_ENEMY_STAB_DAMAGE;

		//If using a weapon, add that weapon's damage.
		if (m_attackType == AttackType::Stabber)
		{
			if (ITEMTYPE_GetItemData(m_heldItem).Actions & TOOL_ACTION::WEAPON)
			{
				std::string damageAttr = ITEMTYPE_GetItemData(m_heldItem).Attributes.at(TOOL_ACTION::WEAPON);
				damage += std::stoi(damageAttr.substr(0, damageAttr.find_first_of(',')));
			}
		}

		if (std::abs((m_position.X + m_scale.X/2.f) - (TestGame::ThePlayer->GetPosition().X + TestGame::ThePlayer->GetScale().X/2.f)) < IMPRESSIONABLE_ENEMY_STAB_DISTANCE && std::abs((m_position.Y + m_scale.Y / 2.f) - (TestGame::ThePlayer->GetPosition().Y + TestGame::ThePlayer->GetScale().Y / 2.f)) < IMPRESSIONABLE_ENEMY_STAB_DISTANCE - 12.f)
		{
			if (m_scale.X < 0.f)
			{
				if (TestGame::ThePlayer->GetPosition().X + TestGame::ThePlayer->GetScale().X >= m_position.X)
				{
					TestGame::ThePlayer->Damage(damage, this, false, true);
				}
			}
			else {
				if (TestGame::ThePlayer->GetPosition().X <= m_position.X + m_scale.X)
				{
					TestGame::ThePlayer->Damage(damage, this, false, true);
				}
			}
		}
	}

}

