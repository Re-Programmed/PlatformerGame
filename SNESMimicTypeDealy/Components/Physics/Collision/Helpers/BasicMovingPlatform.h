#pragma once
#include "StaticBoxCollisionObject.h"
#include "../../../../Utils/Time/GameTime.h"

namespace GAME_NAME::Components::Physics::Collision
{
	/// <summary>
	/// A simple platform that will move any objects colliding with it along its path and will allow objects to travel through the bottom side.
	/// </summary>
	class BasicMovingPlatform : public StaticBoxCollisionObject
	{
	public:
		BasicMovingPlatform(Vec2 position, Vec2 scale, GAME_NAME::Rendering::Sprite* sprite)
			: StaticBoxCollisionObject(position, scale, sprite)
		{
			SetOnCollision(onCollision);
		};

		void Update(GLFWwindow* window);

	protected:
		inline static bool onCollision(Vec2 push, Objects::GameObject* object, Objects::GameObject* collider)
		{
			//Check if the collision occured with the bottom face.
			if (push.Y > 0)
			{
				BasicMovingPlatform* bp = static_cast<BasicMovingPlatform*>(object);
				
				float moveDistanceX = object->GetPosition().X - bp->m_lastPositionX;

				//Move the collided object with the platform, add -0.75 for a bit of a hack that allows the object to stay on when moving down.
				collider->Translate(moveDistanceX, Utils::Time::GameTime::DeltaTime::GetDeltaTime()  * 60.f * -0.75f);

				return true;
			}
			
			//Did not collide with top, cancel collision.
			return false;
		}

		/// <summary>
		/// The position that the platform was in the frame prior to checking all collisions and moving.
		/// </summary>
		float m_lastPositionX = 0.f;
	};
}