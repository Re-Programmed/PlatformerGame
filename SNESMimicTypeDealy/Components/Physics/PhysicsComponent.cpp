#include "PhysicsComponent.h"
#include "../../Utils/Time/GameTime.h"

#define PHYSICS_COMPONENT_TARGET_SPF 0.0166666	//The number of seconds per physics frame.
#define PHYSICS_COMPONENT_VELOCITY_DAMP 0.017f	//Damps velocity each frame.

namespace GAME_NAME
{
	namespace Components
	{
		namespace Physics
		{
			

			void PhysicsComponent::Update(GLFWwindow* window, Objects::GameObject* object)
			{
				m_physicsTicks = 0;
				m_stackedPhysicsTicks += GAME_NAME::Utils::Time::GameTime::GetScaledDeltaTime();

				object->Translate(m_velocity * (const float)GAME_NAME::Utils::Time::GameTime::GetScaledDeltaTime());
				frameTick(window, object);

				if (m_stackedPhysicsTicks >= PHYSICS_COMPONENT_TARGET_SPF)
				{
					while (m_stackedPhysicsTicks >= PHYSICS_COMPONENT_TARGET_SPF)
					{
						m_physicsTicks++;
						if (m_rotationalVelocity)
						{
							object->RotateAboutCenter(m_rotationalVelocity);

							if (m_slideFrames <= 0)
							{
								rotAirDrag();
							}
						}

						if (m_slideFrames <= 0)
						{
							if (m_velocity.X) { xAirDrag(); }
							if (m_velocity.Y) { yAirDrag(); }
						}
						else {
							m_slideFrames--;
						}

						physicsTick(window, object);

						m_stackedPhysicsTicks -= PHYSICS_COMPONENT_TARGET_SPF;
					}
				}
			}

			void PhysicsComponent::xAirDrag()
			{
				if (m_velocity.X <= -m_frictionDrag)
				{
					m_velocity += Vec2(m_frictionDrag, 0);
				}
				else if (m_velocity.X >= m_frictionDrag)
				{
					m_velocity -= Vec2(m_frictionDrag, 0);
				}
				else {
					m_velocity = Vec2(0, m_velocity.Y);
				}
			}

			void PhysicsComponent::yAirDrag()
			{
				if (m_velocity.Y <= -m_airDrag)
				{
					m_velocity += Vec2(0, m_airDrag);
				}
				else if (m_velocity.Y >= m_airDrag)
				{
					m_velocity -= Vec2(0, m_airDrag);
				}
				else {
					m_velocity = Vec2(m_velocity.X, 0);
				}
			}

			void PhysicsComponent::rotAirDrag()
			{
				if (m_rotationalVelocity <= -m_rotationalDrag)
				{
					m_rotationalVelocity += m_rotationalDrag;
				}
				else if (m_rotationalVelocity >= m_rotationalDrag)
				{
					m_rotationalVelocity -= m_rotationalDrag;
				}
				else {
					m_rotationalVelocity = 0;
				}
			}

			void PhysicsComponent::SetVelocity(Vec2 velocity)
			{
				m_velocity = velocity;
			}

			void PhysicsComponent::SetVelocityX(float X)
			{
				m_velocity.X = X;
			}

			void PhysicsComponent::SetVelocityY(float Y)
			{
				m_velocity.Y = Y;
			}

			Vec2 PhysicsComponent::GetVelocity()
			{
				return m_velocity;
			}

			void PhysicsComponent::AddVelocity(Vec2 velocity)
			{
				m_velocity += velocity;
			}

			void PhysicsComponent::SetFrictionDrag(float friction)
			{
				m_frictionDrag = friction;
			}
		}
	}
}