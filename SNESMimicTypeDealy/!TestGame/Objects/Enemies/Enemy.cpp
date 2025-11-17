#include "./Enemy.h"

#include "../../../Utils/Time/GameTime.h"

#include "../../../Resources/Save/SaveManager.h"

#include "../../../Rendering/DynamicSprite.h"

#include "../../../Rendering/Renderers/Renderer.h"

#include "HealthBarRenderer.h"

#define ATTACK_ANIMATION_LENGTH 1.2f

namespace GAME_NAME::Objects::Enemies
{
	std::vector<Enemy*> Enemy::EnemyRegistry;

	void Enemy::Update(GLFWwindow* window)
	{
		if (m_isDead)
		{
			if (m_attackedAnimationTimer > 0.f)
			{
				m_attackedAnimationTimer -= Utils::Time::GameTime::GetScaledDeltaTime();
				return;
			}

			Renderer::DestroyActiveObject(this);
			return;
		}

		if (m_rotation != 0)
		{
			float diff = m_rotation - std::lerp(m_rotation, 0.f, 0.05f);
			SetRotationAboutCenter(diff);
		}

		if (!m_allowPathfinding)
		{
			m_pathfindTimeout.Timer = 0.f;
			goto finish_pathfind;
		}	

		//Ignore Y pathfind? Check X distance.
		if (m_enemyAttributes->IgnoreYPathfind && std::abs(m_position.X - m_pathfind.X) < PATHFINDING_PADDING)
		{
			m_pathfindTimeout.Timer = 0.f;
			goto finish_pathfind;
		}
		
		//distance SCOPE.
		{
			float distance = Vec2::Distance(m_position, m_pathfind); //Distance from destination.

			if (distance <= PATHFINDING_PADDING)
			{
				m_pathfindTimeout.Timer = 0.f;
				goto finish_pathfind;
			}

			//Check if pathfinding has been attempted and failed many times.
			if (std::abs(distance - m_pathfindTimeout.Distance) < PATHFINDING_PADDING)
			{
				m_pathfindTimeout.Timer += Utils::Time::GameTime::GetScaledDeltaTime();
			}
			else {
				m_pathfindTimeout.Timer = 0.f;
			}

			m_pathfindTimeout.Distance = distance;

			std::cout << m_onGround << "\n";

			//Move the enemy towards the pathfinding point if they are not already at it and ensure the enemy dosent exceed terminal velocity.
			if (std::abs(m_physics->GetVelocity().X) < m_enemyAttributes->TerminalMovementSpeed && m_onGround)
			{
				m_physics->AddVelocity(Vec2((m_position.X - m_pathfind.X < 0) ? m_enemyAttributes->MovementSpeed : -m_enemyAttributes->MovementSpeed, 0) * Utils::Time::GameTime::GetScaledDeltaTime());
			}
		}

finish_pathfind:
		ActiveBoxCollisionGravityObject::Update(window);

		updateHealthBar();
	}

	void Enemy::updateHealthBar()
	{
		if (m_health < m_maxHealth && !m_isDead)
		{
			HealthBarRenderer::UpdateHealthBar(this, m_health);
		}
	}

	void Enemy::createHealthBar()
	{
		HealthBarRenderer::CreateHealthBar(this, m_maxHealth, m_health);
	}

	void Enemy::destroyHealthBar()
	{
		HealthBarRenderer::RemoveHealthBar(this);
	}

	void Enemy::Damage(float damage, const Vec2 attackOrigin)
	{
		if (m_isDead) { return; }

		m_health -= damage;
		m_attackedAnimationTimer = ATTACK_ANIMATION_LENGTH;

		if (m_health <= 0.f) { Kill(); return; }

		//Animate damage and add velocity.

		if (attackOrigin.X != 0 && attackOrigin.Y != 0)
		{
			Vec2 outwardVector = Vec2::FindExplosionDestination(attackOrigin, m_position + (m_scale/2.f), 2000.f, 120.f);
			outwardVector.Y = 0;

			std::cout << outwardVector.ToString();

			m_physics->AddVelocity(outwardVector);
		}

		createHealthBar();

		std::cout << "<ENEMY DAMAGED> Health: " << m_health << ".\n";
	}

	void Enemy::Heal(float health)
	{
		m_health += health;
		if (m_health > m_maxHealth)
		{
			m_health = m_maxHealth;
		}
	}

	void Enemy::Kill()
	{
		m_isDead = true;

		destroyHealthBar();
	}

	void Enemy::onCollision(Vec2 push, GameObject* cause)
	{
		if (push.Y > 0)
		{
			m_onGround = true;
		}
		else {
			m_onGround = false;
		}
	}

	Vec4 Enemy_redTextureColor[4] = {
	{1, 0.25, 0.25, 1},
	{1, 0.25, 0.25, 1},
	{1, 0.25, 0.25, 1},
	{1, 0.25, 0.25, 1}
	};

	void Enemy::Render(const Vec2& cameraPostion)
	{
		//Either in the first or last 1/3 of the attacking animation, turn sprite red.
		if ((m_attackedAnimationTimer > ATTACK_ANIMATION_LENGTH * (2.f / 3.f)) || (m_attackedAnimationTimer > 0.f && m_attackedAnimationTimer < ATTACK_ANIMATION_LENGTH * (1.f / 3.f)))
		{
			Rendering::DynamicSprite* redSprite = new Rendering::DynamicSprite(m_sprite->GetSpriteId());

			redSprite->UpdateTextureColor(Enemy_redTextureColor);

			redSprite->Render(cameraPostion, { m_position.X + m_scale.X, m_position.Y + m_scale.Y }, { -m_scale.X, -m_scale.Y });
			delete redSprite;

			m_attackedAnimationTimer -= Utils::Time::GameTime::GetScaledDeltaTime();

			m_didRender = true;
			return;
		}

		if (m_supercharge > 0)
		{
			Rendering::DynamicSprite superchargedSpriteVersion(m_sprite->GetSpriteId());

			//Darken the enemy's color as they become more supercharged.
			float brightenScale = 1 - ((float)(m_supercharge) / 5.f);

			Vec4 blueTextureColor[4] = {
				{ brightenScale, brightenScale, 1, 1 },
				//Purpleish hue.
				{ brightenScale, std::clamp(brightenScale, 0.4f, 1.f) - 0.4f, 1, 1 },
				{ brightenScale, std::clamp(brightenScale, 0.4f, 1.f) - 0.4f, 1, 1 },
				{ brightenScale, brightenScale, 1, 1 }
			};

			superchargedSpriteVersion.UpdateTextureColor(blueTextureColor);
			
			superchargedSpriteVersion.Render(cameraPostion, { m_position.X + m_scale.X, m_position.Y + m_scale.Y }, { -m_scale.X, -m_scale.Y });
			
			m_didRender = true;
			return;
		}

		ActiveBoxCollisionGravityObject::Render(cameraPostion);

	}

	void Enemy::LoadState()
	{
		std::string resultStr("");
		Resources::SaveManager::GetLevelString(resultStr, m_objectSaveID);

		
	}

	void Enemy::SaveState()
	{
		std::string saveStr = "";
		saveStr = saveStr + std::to_string(m_position.X) + ",";
		saveStr = saveStr + std::to_string(m_position.Y) + ",";
		saveStr = saveStr + std::to_string(m_health);

		Resources::SaveManager::SaveLevelString(saveStr, m_objectSaveID);
	}

	Enemy::~Enemy()
	{
		EnemyRegistry.erase(EnemyRegistry.begin() + m_enemyIndex);

		delete m_enemyAttributes;

		ActiveBoxCollisionGravityObject::~ActiveBoxCollisionGravityObject();
	}

	Enemy::EnemyAttributes::EnemyAttributes()
		: InputAttributes(nullptr)
	{

	}
}