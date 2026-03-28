#pragma once
#include "../IComponent.h"
#include "../../Utils/Math/Vec2.h"
namespace GAME_NAME
{
	namespace Components
	{
		namespace Physics
		{
			using namespace MathUtils;
			class PhysicsComponent	//A component that can manage the velocity of an object.
				: public IComponent
			{
			public:
				PhysicsComponent() : m_velocity(Vec2::Zero), m_rotationalVelocity(0.f), m_frictionDrag(1.f), m_airDrag(0.02f), m_rotationalDrag(0.085f) {}
				void Update(GLFWwindow* window, Objects::GameObject* object) final;

				void SetVelocity(Vec2 velocity);

				void SetVelocityX(float X);
				void SetVelocityY(float Y);

				Vec2 GetVelocity();

				void AddVelocity(Vec2 velocity);

				inline void SetRotationalVelocity(float velocity) { m_rotationalVelocity = velocity; }
				inline float GetRotationalVelocity() { return m_rotationalVelocity; }

				inline void AddRotationalVelocity(float velocity) { m_rotationalVelocity += velocity; }

				/// <summary>
				/// Returns the number of times the physics was updated for this component after calling Update().
				/// (Physics is updated at a constant rate. PHYSICS_COMPONENT_TARGET_SPF)
				/// </summary>
				/// <returns></returns>
				const inline unsigned char GetUpdatesThisFrame()
				{
					return m_physicsTicks;
				}

				inline void SlideFor(int frames) { m_slideFrames = frames; }
				inline int GetSliding() { return m_slideFrames; }

				/// <summary>
				/// Sets how much the object will slow down when moving left and right on the ground.
				/// </summary>
				/// <param name="airResistance"></param>
				void SetFrictionDrag(float airResistance);

			protected:
				virtual void physicsTick(GLFWwindow* window, Objects::GameObject* object) = 0; //Called each physics update.
				virtual void frameTick(GLFWwindow* window, Objects::GameObject* object) = 0; //Called each frame update.

				Vec2 m_velocity;
				float m_rotationalVelocity;
				float m_frictionDrag;	//How much X velocity is subtracted each frame.
				float m_airDrag;		//How much Y velocity is subtracted each frame.
				float m_rotationalDrag;	//How much rotational velocity is subtracted each frame.

				int m_slideFrames = 0;	//If the object is supposed to slide (no friction).

				double m_stackedPhysicsTicks = 0;
				unsigned char m_physicsTicks = 0;
			private:
				void xAirDrag();		//Calculate X drag and clamp velocity.
				void yAirDrag();		//Calculate Y drag and clamp velocity.

				void rotAirDrag();		//Calculate Rotational drag and clamp velocity.
			};
		}
	}
}