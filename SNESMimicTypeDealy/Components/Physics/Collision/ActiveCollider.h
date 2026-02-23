#pragma once
#include "../../IComponent.h"
#include "StaticCollider.h"
#include <functional>
#include "../../../Utils/Math/Vec2.h"

namespace GAME_NAME
{
	namespace Components
	{
		namespace Physics
		{
			namespace Collision
			{
				//Simple function for checking if a class is an instance of another.
				template<typename Base, typename T>		
				inline bool instanceof(const T* ptr) {
					return dynamic_cast<const Base*>(ptr) != nullptr; 
				}

				/// <summary>
				/// A collider that is affected by collisions.
				/// </summary>
				class ActiveCollider
					: public IComponent

				{
				public:
					ActiveCollider(bool collideWithOtherActive = true)
						: m_beforeUpdate(nullptr), m_object(nullptr), ID(0), CollideWithOtherActive(collideWithOtherActive)
					{
						m_onCollision = nullptr;
					};

					int ID;
					/// <summary>
					/// If true, this collider will collide with other active colliders.
					/// </summary>
					bool CollideWithOtherActive;

					virtual void Update(GLFWwindow* window, Objects::GameObject* object) = 0;
					virtual void Init(Objects::GameObject* object) final
					{
						m_object = object;
					}

					//Checks if this collider is currently colliding with another collider and calls Translate and m_onCollision appropriately.
					virtual void CheckStaticCollisionShapes(StaticCollider* collider) = 0;
					//Checks if this collider is currently colliding with another ACTIVE collider and calls Translate and m_onCollision appropriately. Might also apply a backwards force.
					virtual void CheckActiveCollisionShapes(ActiveCollider* collider) = 0;

					//Set the callback for when this object collides with another.
					inline void SetOnCollision(void(*onCollision)(GAME_NAME::MathUtils::Vec2 push, Objects::GameObject*, Objects::GameObject*))
					{
						m_onCollision = onCollision;
					}

					//Set the callback for what should happen just before any collisions are calculated. (Most physics calculations should go here.)
					void SetBeforeUpdate(void(*beforeUpdate)(Objects::GameObject*))
					{
						m_beforeUpdate = beforeUpdate;
					}

					inline void BeforeUpdate()
					{
						if (m_beforeUpdate != nullptr)
						{
							m_beforeUpdate(m_object);
						}
					}

					Objects::GameObject* GetObject()
					{
						return m_object;
					}

				protected:
					Objects::GameObject* m_object;
					void(*m_onCollision)(GAME_NAME::MathUtils::Vec2 push, Objects::GameObject* self, Objects::GameObject* other);
					void(*m_beforeUpdate)(Objects::GameObject*);
				};
			}
		}
	}
}