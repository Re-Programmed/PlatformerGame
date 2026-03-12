#include "AngryFarmer.h"

#include "../../../TestGame.h"

#include "../../../../Utils/Time/GameTime.h"

#define ANGRY_FARMER_X_AGRESSION_DISTANCE 50
#define ANGRY_FARMER_Y_AGRESSION_DISTANCE 20

#define ANGRY_FARMER_CHASE_SPEED_MULT 2.92f

#define ANGRY_FARMER_STOP_DISTANCE 60.f

#define ANGRY_FARMER_WALK_ANIMATION { 1, 2, 3, 2, 1, 4, 5, 4 }
#define ANGRY_FARMER_ATTACK_ANIMATION { 6, 7, 6, 7 }

#define ANGRY_FARMER_DEFAULT_SPEED 65.f

#define ANGRY_FARMER_DAMAGE 10.f
#define ANGRY_FARMER_ATTACK_COOLDOWN 3.0


namespace GAME_NAME::Objects::Enemies
{
	using namespace Components::Animation;

	void AngryFarmer::Update(GLFWwindow* window)
	{
		if (m_isDead)
		{
			Enemy::Update(window);
			return;
		}

		if (m_attacking > 0.0)
		{
			m_attacking -= Utils::Time::GameTime::GetScaledDeltaTime();
			calculateAttack();
		}

		
		if (m_isAggrivated)
		{
			//Angry at the player, chase the player.

			if (Vec2::Distance(TestGame::ThePlayer->GetPosition(), m_position + m_scale / 2.f) <= ANGRY_FARMER_STOP_DISTANCE)
			{
				Enemy::Update(window);
				updateTextures();				
				lunge();

				return;
			}

			setPathfinding({ TestGame::ThePlayer->GetPosition().X, m_position.Y });
			updateTextures();
			Enemy::Update(window);

			return;
		}

		
		if (std::abs(TestGame::ThePlayer->GetPosition().X - m_position.X) < ANGRY_FARMER_X_AGRESSION_DISTANCE && std::abs(TestGame::ThePlayer->GetPosition().Y - m_position.Y) < ANGRY_FARMER_Y_AGRESSION_DISTANCE)
		{
			m_isAggrivated = true;

			//Play aggro animation.

			//Increase speed.
			m_enemyAttributes->MovementSpeed *= ANGRY_FARMER_CHASE_SPEED_MULT;
			m_enemyAttributes->TerminalMovementSpeed *= ANGRY_FARMER_CHASE_SPEED_MULT;
		}
		
		updateTextures();

		LeftRightEnemy::Update(window);
	}

	void AngryFarmer::onCollision(Vec2 push, GameObject* self, GameObject* other)
	{
		LeftRightEnemy::onCollision(push, self, other);

		m_physics->SetVelocity({ m_physics->GetVelocity().X, 0.f});
	}

	AngryFarmer::AngryFarmer(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, Vec2 leftAnchor, Vec2 rightAnchor, AngryFarmerAttributes* attributes, size_t saveID)
		: LeftRightEnemy(position, scale, sprite, leftAnchor, rightAnchor, attributes, saveID), m_animatorComponent(nullptr)
	{
		std::vector<std::shared_ptr<GAME_NAME::Components::Animation::Animation>> animations;

		std::vector<std::shared_ptr<Sprite>> walkAnimation;
		std::vector<std::shared_ptr<Sprite>> attackAnimation;

		for (int i : ANGRY_FARMER_WALK_ANIMATION)
		{
			walkAnimation.emplace_back(std::make_shared<Sprite>(m_sprite->GetSpriteId() + i));
		}

		for (int i : ANGRY_FARMER_ATTACK_ANIMATION)
		{
			attackAnimation.emplace_back(std::make_shared<Sprite>(m_sprite->GetSpriteId() + i));
		}

		animations.push_back(std::make_shared<GAME_NAME::Components::Animation::Animation>(AnimData(walkAnimation), ANIM_12_SPF));
		animations.push_back(std::make_shared<GAME_NAME::Components::Animation::Animation>(AnimData(attackAnimation), ANIM_6_SPF));

		m_animatorComponent = new AnimatorComponent(animations);

		m_physics->SetFrictionDrag(0.4f);

		m_enemyAttributes->MovementSpeed = ANGRY_FARMER_DEFAULT_SPEED;
		m_enemyAttributes->TerminalMovementSpeed = 50.f;
		m_enemyAttributes->IgnoreYPathfind = true;

	}

	AngryFarmer::~AngryFarmer()
	{
		delete m_animatorComponent;
	}

	void AngryFarmer::lunge()
	{
		//Still on cooldown.
		if (m_attacking > 0.0) { return; }

		m_hasHitPlayerThisAttack = false;

		//Lunge Attack.
		if (TestGame::ThePlayer->GetPosition().X > m_position.X + m_scale.X / 2.f)
		{
			m_physics->AddVelocity(Vec2{ 15.f, 0.f });
		}
		else {
			m_physics->AddVelocity(Vec2{ -15.f, 0.f });
		}

		m_attacking = ANGRY_FARMER_ATTACK_COOLDOWN;
	}

	void AngryFarmer::calculateAttack()
	{
		if (Vec2::Distance(TestGame::ThePlayer->GetPosition(), m_position) < 10.f && !m_hasHitPlayerThisAttack && m_attacking > ANGRY_FARMER_ATTACK_COOLDOWN/2.6)
		{
			TestGame::ThePlayer->Damage(ANGRY_FARMER_DAMAGE, this, false, false);
			m_hasHitPlayerThisAttack = true;
		}
	}

	void AngryFarmer::updateTextures()
	{

		//Flip texture if moving backwards (and not already flipped).
		if ((m_physics->GetVelocity().X < 0.f && m_scale.X > 0) || (m_physics->GetVelocity().X > 0.f && m_scale.X < 0))
		{
			m_scale.X = -m_scale.X;

			//Update position since scale flip is along the side edge -- not middle.
			m_position.X -= m_scale.X;
		}

		if (m_attacking > ANGRY_FARMER_ATTACK_COOLDOWN - (ANIM_6_SPF * 7.0))
		{
			m_animatorComponent->SetCurrentAnimation(1);	//Attack animation.
			m_animatorComponent->SetSpeedMult(1.0);
			//Is attacking.
			m_animatorComponent->Update(TestGame::INSTANCE->FirstWindow, this);
			return;
		}



		float anim_momentum = std::abs(m_physics->GetVelocity().X);

		if (anim_momentum > 0.1f && m_onGround)
		{
			m_animatorComponent->SetSpeedMult((double)anim_momentum / 80.0);
			m_animatorComponent->SetCurrentAnimation(0);  //Walking Animation
		}

		m_animatorComponent->Update(TestGame::INSTANCE->FirstWindow, this);
	}
}
