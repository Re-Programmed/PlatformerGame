#pragma once

#include "../RotationalBoxCollider.h"
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



				class ActiveRotationalBoxCollisionGravityObject	//An object that has a box collider, rotates on collision, and is affected by gravity.
					: public Objects::GameObject
				{
				public:

					struct RotationalCollider_Settings
					{
						float Bounce;
						float RotationEffect;
						float RotationGravityEffect;

						RotationalCollider_Settings(float b, float re, float rge) : Bounce(b), RotationEffect(re), RotationGravityEffect(rge) {}
					};
					
					ActiveRotationalBoxCollisionGravityObject(Vec2 position, Vec2 scale, GAME_NAME::Rendering::Sprite* sprite, RotationalCollider_Settings settings)
						: GameObject(position, scale, sprite), m_boxCollider(new RotationalBoxCollider()), m_physics(new GravityComponent()), m_settings(settings)
					{
						m_boxCollider->Init(this);
						m_physics->Init(this);
						m_boxCollider->SetOnCollision(onCollisionS);
						m_boxCollider->SetBeforeUpdate(beforeUpdate);
					}

					ActiveRotationalBoxCollisionGravityObject(Vec2 position, Vec2 scale, std::shared_ptr<GAME_NAME::Rendering::Sprite> sprite, RotationalCollider_Settings settings)
						: GameObject(position, scale, sprite), m_boxCollider(new RotationalBoxCollider()), m_physics(new GravityComponent()), m_settings(settings)
					{
						m_boxCollider->Init(this);
						m_physics->Init(this);
						m_boxCollider->SetOnCollision(onCollisionS);
						m_boxCollider->SetBeforeUpdate(beforeUpdate);
					}


					~ActiveRotationalBoxCollisionGravityObject()
					{
						//delete m_boxCollider;
						//delete m_physics;

						GameObject::~GameObject();
					}

					bool GetActive()
					{
						return true;
					}

					void Update(GLFWwindow* window)
					{
						CollisionManager::RegisterActiveColliderToBuffer(m_boxCollider);
					}

					void SetGravity(const float gVelocity)
					{
						m_physics->SetGravitationalVelocity(gVelocity);
					}

				protected:
					const RotationalCollider_Settings m_settings;

					RotationalBoxCollider* const m_boxCollider;
					GravityComponent* const m_physics;			//Used for gravity and velocity.

					static void onCollisionS(Vec2 push, Objects::GameObject* obj, Objects::GameObject* other)
					{
						ActiveRotationalBoxCollisionGravityObject* acgo = ((ActiveRotationalBoxCollisionGravityObject*)obj);
						acgo->onCollision(push, obj, other);
						acgo->m_physics->AddRotationalVelocity(-push.X * acgo->m_settings.RotationEffect * std::abs(acgo->m_physics->GetVelocity().X/90));
						if (push.Y > 0)
						{ 
							acgo->SetGravity(0.f); //Resets the gravity to 0.

							int halfRots = (int)acgo->m_rotation / 45;
							if (std::abs((int)acgo->m_rotation % 90) >= acgo->m_settings.RotationGravityEffect)
							{
								acgo->RotateAboutCenter((acgo->m_rotation/std::abs(acgo->m_rotation)) * -acgo->m_settings.RotationGravityEffect * (halfRots % 2 ? -1 : 1));
							}
							else {
								acgo->RotateAboutCenter(-acgo->m_rotation + halfRots*45);
							}


						}		
						else if (push.X != 0) { acgo->m_physics->SetVelocityX(push.X * acgo->m_settings.Bounce); }	//If you hit the side of an object, set the X velocity to the bounce factor.
					}

					/// <summary>
					/// Override to run code on collision.
					/// </summary>
					/// <param name="push">The direction the object is getting pushed to process the collision.</param>
					virtual void onCollision(Vec2 push, GameObject* other, GameObject* self)
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
						ActiveRotationalBoxCollisionGravityObject* acgo = ((ActiveRotationalBoxCollisionGravityObject*)obj);
						acgo->beforeCollision();
						acgo->m_physics->Update(nullptr, obj);
					}
				};

			}
		}
	}
}