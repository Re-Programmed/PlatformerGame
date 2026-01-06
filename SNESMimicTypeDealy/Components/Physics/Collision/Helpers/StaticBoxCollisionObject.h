#include "../StaticBoxCollider.h"
#include "../CollisionManager.h"

#ifndef _STATIC_BOX_COLLISION_OBJECT
#define _STATIC_BOX_COLLISION_OBJECT


namespace GAME_NAME
{
	namespace Components
	{
		namespace Physics
		{
			namespace Collision
			{
				using namespace MathUtils;

				//A collision object that will not be affected by collisions. (ex. The ground, it still collides but it does nothing when it collides.)
				class StaticBoxCollisionObject		
					: public Objects::GameObject
				{
				public:
					/// <summary>
					/// 
					/// </summary>
					/// <param name="position"></param>
					/// <param name="scale"></param>
					/// <param name="sprite"></param>
					/// <param name="adjustedColliderScale">Used to change the size of the collider without changing the size of the object.</param>
					StaticBoxCollisionObject(Vec2 position, Vec2 scale, GAME_NAME::Rendering::Sprite* sprite, Vec2 adjustedColliderScale = NULL)
						: GameObject(position, adjustedColliderScale, sprite), m_boxCollider(new StaticBoxCollider()), m_trueRenderScale(scale)
					{
						if (m_scale.X == 0) { m_scale = scale; }

						m_boxCollider->Init(this);
					}

					~StaticBoxCollisionObject()
					{
						delete m_boxCollider;
					}

					void Update(GLFWwindow* window)
					{
						if (!m_enabled) { return; }
						CollisionManager::RegisterStaticColliderToBuffer(m_boxCollider);
					}

					inline void SetBeforeCollision(void(*beforeCollision)(GameObject* self))
					{
						m_boxCollider->SetBeforeCollision(beforeCollision);
					}

					inline void SetOnCollision(bool(*onCollision)(Vec2, Objects::GameObject*, Objects::GameObject*))
					{
						m_boxCollider->SetOnCollision(onCollision);
					}

					inline void SetEnabled(bool enabled)
					{
						m_enabled = enabled;
					}

					/// <summary>
					/// Returns the scale that the sprite is rendered at if the scale of the collider has been modified.
					/// </summary>
					/// <returns></returns>
					inline Vec2 GetDisplayScale()
					{
						return m_trueRenderScale;
					}

					inline void SetDisplayScale(Vec2 displayScale)
					{
						m_trueRenderScale = displayScale;
					}

					void Render(const Vec2& cameraPosition) override
					{
						if (m_trueRenderScale.X != m_scale.X || m_trueRenderScale.Y != m_scale.Y)
						{
							Vec2 colliderScale = this->m_scale;
							this->m_position -= Vec2{ 0.f, colliderScale.Y - colliderScale.Y };
							this->m_scale = m_trueRenderScale;
							GameObject::Render(cameraPosition);
							this->m_position += Vec2{ 0.f, colliderScale.Y - colliderScale.Y };
							this->m_scale = colliderScale;
						}
						else {
							GameObject::Render(cameraPosition);
						}
					}

				private:
					StaticBoxCollider* const m_boxCollider;

					bool m_enabled = true;

					Vec2 m_trueRenderScale;
				};
			}
		}
	}
}

#endif