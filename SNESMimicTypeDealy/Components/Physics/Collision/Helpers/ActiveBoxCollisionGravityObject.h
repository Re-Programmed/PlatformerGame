#pragma once

#include "../ActiveBoxCollider.h"
#include "../CollisionManager.h"
#include "../../GravityComponent.h"

namespace GAME_NAME
{
	namespace Components
	{
		namespace Physics
		{
			namespace Collision
			{
				using namespace MathUtils;



				class ActiveBoxCollisionGravityObject	//An object that has a box collider, and is affected by gravity.
					: public Objects::GameObject
				{
				public:

					ActiveBoxCollisionGravityObject(Vec2 position, Vec2 scale, GAME_NAME::Rendering::Sprite* sprite)
						: GameObject(position, scale, sprite), m_boxCollider(new ActiveBoxCollider()), m_physics(new GravityComponent())
					{
						m_boxCollider->Init(this);
						m_physics->Init(this);
						m_boxCollider->SetOnCollision(onCollisionS);
						m_boxCollider->SetBeforeUpdate(beforeUpdate);
					}

					ActiveBoxCollisionGravityObject(Vec2 position, Vec2 scale, std::shared_ptr<GAME_NAME::Rendering::Sprite> sprite)
						: GameObject(position, scale, sprite), m_boxCollider(new ActiveBoxCollider()), m_physics(new GravityComponent())
					{
						m_boxCollider->Init(this);
						m_physics->Init(this);
						m_boxCollider->SetOnCollision(onCollisionS);
						m_boxCollider->SetBeforeUpdate(beforeUpdate);
					}

					~ActiveBoxCollisionGravityObject()
					{
						//delete m_boxCollider;
						//delete m_physics;
					}

					bool GetActive()
					{
						return true;
					}
					
					void Update(GLFWwindow* window)
					{
						if (m_didRender)
						{
							CollisionManager::RegisterActiveColliderToBuffer(m_boxCollider);
						}
						m_didRender = false;
					}

					void SetGravity(const float gVelocity)
					{
						m_physics->SetGravitationalVelocity(gVelocity);
					}

					inline Vec2 GetVelocity()
					{
						return m_physics->GetVelocity() + Vec2(0, m_physics->GetGravitationalVelocity());
					}

					inline GravityComponent* const GetPhysics()
					{
						return m_physics;
					}

					void Render(const Vec2& cameraPosition) override
					{
						m_didRender = true;
						GameObject::Render(cameraPosition);
					}

					inline ActiveBoxCollider* const& GetActiveBoxCollider()
					{
						return m_boxCollider;
					}

				protected:
					ActiveBoxCollider* const m_boxCollider;
					GravityComponent* const m_physics;			//Used for gravity and velocity.

					/// <summary>
					/// True if the object rendered on this frame. (Disable collisions and gravity if the object didn' render since inactive objects will not update when off screen, meaning
					/// active objects will not be able to acurletly reflect collisions.
					/// </summary>
					bool m_didRender = false;

					static void onCollisionS(Vec2 push, Objects::GameObject* obj, Objects::GameObject* other)
					{
						ActiveBoxCollisionGravityObject* acgo = ((ActiveBoxCollisionGravityObject*)obj);
						acgo->onCollision(push, obj, other);

						if (push.Y > 0) { acgo->SetGravity(0.f); }		//Resets the gravity to 0.
						else if (push.X != 0) { acgo->m_physics->SetVelocityX(0.f); }	//If you hit the side of an object, set the X velocity to 0.
					}

					/// <summary>
					/// Override to run code on collision.
					/// </summary>
					/// <param name="push">The direction the object is getting pushed to process the collision.</param>
					virtual void onCollision(Vec2 push, Objects::GameObject* gameObject, Objects::GameObject* other)
					{

					}

					/// <summary>
					/// Called before collision checks.
					/// </summary>
					virtual void beforeCollision()
					{

					}

					//Ensures to update the physics before updating the collisions.
					static void beforeUpdate(Objects::GameObject* obj)
					{
						ActiveBoxCollisionGravityObject* acgo = ((ActiveBoxCollisionGravityObject*)obj);
						acgo->beforeCollision();
						
						if (acgo->m_didRender)
						{
							acgo->m_physics->Update(nullptr, obj);
						}
					}
				};

			}
		}
	}
}