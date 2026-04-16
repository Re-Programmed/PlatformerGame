#include "SlipperySurface.h"

#include "../../Components/Physics/Collision/Helpers/ActiveBoxCollisionGravityObject.h"

namespace GAME_NAME::Objects
{
	SlipperySurface::SlipperySurface(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float slipFactor)
		: StaticBoxCollisionObject(position, scale, sprite), m_slipFactor(slipFactor)
	{
		SetOnCollision(onCollision);
	}

	bool SlipperySurface::onCollision(GAME_NAME::MathUtils::Vec2 push, Objects::GameObject* self, Objects::GameObject* other)
	{
		if (push.Y <= 0.f)
		{
			return true;
		}

		ActiveBoxCollisionGravityObject* otherPhysics = dynamic_cast<ActiveBoxCollisionGravityObject*>(other);

		if (otherPhysics)
		{
			if (reinterpret_cast<SlipperySurface*>(self)->m_slipFactor > 0.f)
			{
				otherPhysics->GetPhysics()->AddVelocity(Vec2{ 0.f, -1.f });
				otherPhysics->GetPhysics()->SlideFor(3);
				return true;
			}

			otherPhysics->GetPhysics()->AddVelocity(Vec2{ -otherPhysics->GetVelocity().X/50.f, 0.f });
		}

		return true;
	}
}
