#pragma once

#include "../../../Components/Physics/Collision/Helpers/ActiveBoxCollisionGravityObject.h"

#include "../../../Objects/GameObjectState.h"

constexpr uint8_t PATHFINDING_PADDING = 1;

namespace GAME_NAME::Objects::Enemies
{
	using namespace Components::Physics::Collision;

	class Enemy
		: public ActiveBoxCollisionGravityObject, public GameObjectState
	{
	public:
		static std::vector<Enemy*> EnemyRegistry;

		struct EnemyAttributes
		{
			float MovementSpeed = 5.0F;
			float TerminalMovementSpeed = 20.F;
			
			bool IgnoreYPathfind = false;

			struct TempAttributes
			{
				float Gravity = 1.5F;
				float FrictionDrag = 4.F;
			};

			TempAttributes* InputAttributes;

			EnemyAttributes();

			EnemyAttributes(const EnemyAttributes& enemyAttributes)
				: MovementSpeed(enemyAttributes.MovementSpeed), TerminalMovementSpeed(enemyAttributes.TerminalMovementSpeed), InputAttributes(enemyAttributes.InputAttributes)
			{}

			EnemyAttributes(EnemyAttributes&& enemyAttributes) noexcept
				: MovementSpeed(enemyAttributes.MovementSpeed), TerminalMovementSpeed(enemyAttributes.TerminalMovementSpeed), InputAttributes(enemyAttributes.InputAttributes)
			{}

			EnemyAttributes(float movementSpeed, float terminalMovementSpeed, TempAttributes* tempAttributes)
				: MovementSpeed(movementSpeed), TerminalMovementSpeed(terminalMovementSpeed), InputAttributes(tempAttributes)
			{}

			TempAttributes GetInputAttributes()
			{
				if (InputAttributes == nullptr)
				{
					return TempAttributes();
				}

				TempAttributes attribs = *InputAttributes;
				return attribs;
				delete InputAttributes;
			}

			~EnemyAttributes()
			{
				delete InputAttributes;
			}
		};

		Enemy(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, EnemyAttributes* attributes = new EnemyAttributes(), size_t saveId = 0, float health = 20.f, bool allowPathfinding = true)
			: ActiveBoxCollisionGravityObject(position, scale, sprite), GameObjectState(saveId), m_pathfind(position), m_enemyAttributes(attributes), m_pathfindTimeout{ 0.0 }, m_health(health), m_maxHealth(health), m_allowPathfinding(allowPathfinding)
		{
			EnemyAttributes::TempAttributes startAttributes = attributes->GetInputAttributes();
			m_physics->SetGravityStrength(startAttributes.Gravity);
			m_physics->SetFrictionDrag(startAttributes.FrictionDrag);

			//Add enemy to registry for checking attacks and more.
			m_enemyIndex = EnemyRegistry.size();
			EnemyRegistry.push_back(this);
		}

		void Update(GLFWwindow* window) override;

		void Damage(float damage, const Vec2 attackOrigin = Vec2(0));
		void Heal(float health);

		void LoadState() override;
		void SaveState() override;

		void Render(const Vec2& cameraPosition) override;
		
		void Kill();

		inline void AddVelocity(Vec2 velocity)
		{
			m_physics->AddVelocity(velocity);
		}

		inline void AddRotationalVelocity(float rotVelocity)
		{
			m_physics->AddRotationalVelocity(rotVelocity);
		}

		/// <summary>
		/// Increases the enemies power, can provide some scaling factor if you want.
		/// </summary>
		inline void Supercharge(uint8_t scalingFactor)
		{
			//Don't supercharge if already supercharged more.
			if (m_supercharge >= scalingFactor) { return; }

			m_supercharge = scalingFactor;
		}
		
		inline void RemoveSupercharge()
		{
			m_supercharge = 0;
		}

		void onCollision(Vec2 push, GameObject* cause) override;
	protected:
		/// <summary>
		/// Occurs when an enemy is zapped by electricity.
		/// </summary>
		uint8_t m_supercharge = 0;

		bool m_onGround = false;

		bool m_isDead = false; //Is dead?

		/// <summary>
		/// If false, the enemy will not try to pathfind anywhere.
		/// </summary>
		bool m_allowPathfinding;

		/// <summary>
		/// Used for timing the attacked animation.
		/// </summary>
		float m_attackedAnimationTimer = 0.f;

		/// <summary>
		/// Returns true if the enemy has reached its destination.
		/// </summary>
		/// <returns></returns>
		inline bool completedPathfinding() { return m_pathfindTimeout.Timer > 2.0 ? true : 
													((m_enemyAttributes->IgnoreYPathfind ?
														(std::abs(m_position.X - m_pathfind.X) <= PATHFINDING_PADDING) : 
														(Vec2::Distance(m_pathfind, m_position) <= PATHFINDING_PADDING))); }

		float m_health;
		/// <summary>
		/// The health the enemy started with. Healing the enemy will never allow its health to exceed this value.
		/// </summary>
		const float m_maxHealth;

		inline void setPathfinding(Vec2 position)
		{
			this->m_pathfind = position;
		}

		/// <summary>
		/// Called when the health bar of this enemy should be rendered. Can be overriden to customize the health bar rendering.
		/// </summary>
		virtual void updateHealthBar();
		/// <summary>
		/// Called when the enemy is first damaged to create the health bar.
		/// </summary>
		virtual void createHealthBar();
		/// <summary>
		/// Called to disable the enemy health bar until create health bar is called again.
		/// </summary>
		virtual void destroyHealthBar();

		~Enemy();

		EnemyAttributes* m_enemyAttributes;
	private:
		int m_enemyIndex; //This enemy's index in the registry.


		Vec2 m_pathfind;

		/// <summary>
		/// Used to track if the enemy is not making any progress. If so the enemy will time out and complete automatically.
		/// </summary>
		union
		{
			double Timer;
			float Distance;
		} m_pathfindTimeout;
	};
}