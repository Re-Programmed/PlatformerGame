#include "ImpressionableEnemy.h"

#include "../../../../Utils/Time/GameTime.h"

#include "../../../TestGame.h"

#include "../../../../Audio/SoundEvents.h"

#include "../../Environment/Effects/ThoughtEffect.h"


#include "../../../Objects/Projectile.h"

namespace GAME_NAME::Objects::Enemies
{
	using namespace Components::Animation;

	struct ImpressionableAttackType {
		Vec2 Scale;
		std::vector<int> WalkAnimation;
		std::vector<int> AttackAnimation;
		int IdleSprite, UsingPhoneSprite;

		int ReactionImage;

		float MovementSpeed;
		float TerminalMovementSpeed;
	};

	const ImpressionableAttackType IMPRESSIONABLE_ATTACK_TYPES[3] = {
		{ Vec2{ 34.f/2.f, 44.f/2.f }, { 159, 160, 159, 161 }, { 162, 163 }, 157, 164, /*Reaction Image:*/ 166, 500.f, 2750.f},
		{ Vec2{ 34.f/2.f, 44.f/2.f }, { 169, 170, 169, 171 }, { 172, 173 }, 167, 174, 166, 315.f, 1875.f },
		{ Vec2{ 16.f, 16.f }, { 0 }, { 0 }, 0, 0, 0, 5.f, 20.f }
	};

	ImpressionableEnemy::ImpressionableEnemy(Vec2 position, AttackType type, LoadableEnemy::LoadMode loadCondition, bool initiallyAngry, size_t saveId)
		: Enemy(position, IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(type)].Scale, nullptr, new EnemyAttributes(IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(type)].MovementSpeed, IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(type)].TerminalMovementSpeed, new EnemyAttributes::TempAttributes()), saveId),
		m_home(position), m_actionTimer(0.0), m_attackCharge(0.0), m_attackType(type), m_heldItem(ITEM_TYPE::NULL_ITEM),
		LoadableEnemy(loadCondition)
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

		//The enemy will set to be angered if it should load on initilization.
		this->m_isAngered = false;
	}

	void ImpressionableEnemy::Update(GLFWwindow* window)
	{
		LoadableEnemy::Update(m_position, m_scale);

		//Don't do anything until loaded.
		if (!m_hasLoaded) { return; }

		Enemy::Update(window);
		m_animator->Update(window, this);

		updateMotion();
		updateTextures();
	}


#define IMPRESSIONABLE_ENEMY_STAB_DISTANCE 42.5f
#define IMPRESSIONABLE_ENEMY_ATTACK_INTERVAL 1.0

#define IMPRESSIONABLE_ENEMY_PHONE_PROBABILITY 0.005

#define IMPRESSIONABLE_ENEMY_STAB_DAMAGE 7

	void ImpressionableEnemy::Render(const Vec2& cameraPos)
	{
		//Don't render if not loaded.
		if (!m_hasLoaded) { return; }

		if (m_sprite == nullptr) { return; }

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


		if (m_heldItem != ITEM_TYPE::NULL_ITEM && m_isAngered)
		{
			Sprite* heldItemSprite = ITEMTYPE_GetItemTypeTexture(m_heldItem);
			float xOffset = m_scale.X / 2.f + ((m_animator->GetCurrentAnimationIndex() >= 0 && m_animator->GetCurrentAnimation()->GetFrame() % 2 == 0) ? -1.f : 0.f);
			float yOffset = 3.5f;

			if (m_animator->GetCurrentAnimationIndex() == 1 /*Attack animation playing*/)
			{
				yOffset += 2.5f;
				xOffset += m_scale.X < 0.f ? -2.5f : 2.5f;
			}

			heldItemSprite->Render(cameraPos, m_position + Vec2{ xOffset, yOffset }, Vec2{8.f * (m_scale.X < 0.f ? -1.f : 1.f), 8.f});
			delete heldItemSprite;
		}

	}


	void ImpressionableEnemy::updateMotion()
	{
		//Don't move unless angered.
		if (!m_isAngered)
		{
			this->m_physics->SetVelocityX(0.f);

			if (Vec2::Distance(TestGame::ThePlayer->GetPosition(), m_position) < 75.f)
			{
				if ((static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)) <= IMPRESSIONABLE_ENEMY_PHONE_PROBABILITY && m_checkingPhone <= 0.0)
				{
					m_checkingPhone = 4.0;
					Sprite* phoneTexture = ITEMTYPE_GetItemTypeTexture(ITEM_TYPE::PHONE);
					ThoughtEffect::CreateThoughtEffect(m_position + (m_scale * 0.75f), phoneTexture->GetSpriteId(), 1.33);
					delete phoneTexture;
				}
			}
		}

		/*Angry if player near.
		if (!m_isAngered && Vec2::Distance(TestGame::ThePlayer->GetPosition(), m_position) < 45.f)
		{
			m_isAngered = true;
		}*/

		//If close enough and a melee attack, start charging attack.
		if (m_attackType == AttackType::Stabber)
		{
			if (m_actionTimer <= 0.0 && m_attackCharge <= 0.0 && Vec2::Distance(TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale()/2.f, m_position + m_scale / 2.f) < IMPRESSIONABLE_ENEMY_STAB_DISTANCE)
			{
				m_attackCharge += 0.1;
			}
		}

		//Attack on an interval.
		if (m_attackType == AttackType::Thrower)
		{
			float distToPlayer = Vec2::Distance(TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale() / 2.f, m_position + m_scale / 2.f);
			if (m_actionTimer <= 0.f && m_attackCharge <= 0.f && m_isAngered && distToPlayer < TargetResolutionX/0.90f && TestGame::INSTANCE->GetCamera()->GlobalToUI(m_position).X > 0.f /*On screen.*/)
			{
				//Don't throw if moving really fast or the player is on top of them.
				if (distToPlayer >= m_scale.X && m_physics->GetVelocity().X < 80.f)
				{
					m_attackCharge += 0.1f;
				}
			}
		}

		double dt = Utils::Time::GameTime::DeltaTime::GetDeltaTime();

		if (m_checkingPhone > 0.0)
		{
			m_checkingPhone -= dt;
			setPathfinding(m_position);

			//Done checking.
			if (m_checkingPhone <= 0.0)
			{
				m_isAngered = true;
			}

			return;
		}

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
			if (m_attackType == AttackType::Thrower)
			{
				//Don't get too close to the player.
				if (Vec2::Distance(m_position, TestGame::ThePlayer->GetPosition()) > 30.f)
				{
					setPathfinding(TestGame::ThePlayer->GetPosition());
				}
				else {
					setPathfinding(m_position);
				}
			}
			else {
				setPathfinding(TestGame::ThePlayer->GetPosition());
			}
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
		if (m_checkingPhone > 2.0)
		{
			m_sprite.reset(Renderer::GetSprite(IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(m_attackType)].UsingPhoneSprite));
			return;
		}
		else if (m_checkingPhone > 0.0)
		{
			Sprite* sprite(Renderer::GetSprite(IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(m_attackType)].UsingPhoneSprite + 1));

			//First time setting the sprite here.
			if (m_sprite->GetSpriteId() != sprite->GetSpriteId())
			{
				ThoughtEffect::CreateThoughtEffect(m_position + (m_scale * 0.75f), Renderer::GetSpriteIdFromTextureId(IMPRESSIONABLE_ATTACK_TYPES[static_cast<int>(m_attackType)].ReactionImage), 1.33);
			}

			m_sprite.reset(sprite);
			return;
		}

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

			if (std::abs((m_position.X + m_scale.X / 2.f) - (TestGame::ThePlayer->GetPosition().X + TestGame::ThePlayer->GetScale().X / 2.f)) < IMPRESSIONABLE_ENEMY_STAB_DISTANCE && std::abs((m_position.Y + m_scale.Y / 2.f) - (TestGame::ThePlayer->GetPosition().Y + TestGame::ThePlayer->GetScale().Y / 2.f)) < IMPRESSIONABLE_ENEMY_STAB_DISTANCE - 12.f)
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
		else if(m_attackType == AttackType::Thrower) {
			if (ITEMTYPE_GetItemData(m_heldItem).Actions & TOOL_ACTION::RANGED_WEAPON)
			{
				const ItemData& heldItemData = ITEMTYPE_GetItemData(m_heldItem);
				const std::string& attribute = heldItemData.Attributes.at(TOOL_ACTION::RANGED_WEAPON);
				std::stringstream attributeStream(attribute);
				std::string value; uint8_t i = 0;
				int damage = 0; float range = 0; uint8_t projectileType = 0;

				while (std::getline(attributeStream, value, ','))
				{
					switch (i++)
					{
					case 0:
						damage = std::stoi(value);
						break;
					case 1:
						break;
					case 2:
						//Cooldown.
						m_actionTimer = std::stof(value);

						if (m_heldItem == ITEM_TYPE::BOMB || m_heldItem == ITEM_TYPE::GRENADE)
						{
							m_actionTimer *= 4.f;
						}

						break;
					case 3:
						//range = std::stof(value); Ignore this because enemy doesn't charge up the attack to a max but calculates how far to throw.
						break;
					case 4:
						projectileType = std::stoi(value);
						break;
					}
				}

				Vec2 direction = TestGame::ThePlayer->GetPosition() - (m_position + m_scale / 2.f);
				bool throwLeft = direction.X < 0.f;
				range = std::sqrtf(std::abs(direction.X)) * 1.166f;

				GAME_NAME::Objects::Projectile* projectile = new GAME_NAME::Objects::Projectile(throwLeft ? (m_position + Vec2{ -10.f, m_scale.Y / 2.f }) : (m_position + Vec2{ m_scale.X, m_scale.Y / 2.f }), damage, range, projectileType, throwLeft);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(projectile, true, 1, false));
			}
		}
	}

	void ImpressionableEnemy::load()
	{
		this->m_isAngered = true;

		if (getLoadMode() == LoadMode::WhenOffscreen)
		{
			this->m_physics->AddVelocity(Vec2{ 245.f, 40.f });
		}
	}

	void ImpressionableEnemy::unload()
	{
		this->m_isAngered = false;
	}

	void ImpressionableEnemy::onCollision(Vec2 push, GameObject* self, GameObject* other)
	{
		//No bouncy.
		if (push.Y > 0.f)
		{
			m_physics->SetVelocityY(0.f);
		}

		Enemy::onCollision(push, self, other);
	}

}

