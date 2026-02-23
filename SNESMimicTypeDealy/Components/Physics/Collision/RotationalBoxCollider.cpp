#include "RotationalBoxCollider.h"
#include "StaticBoxCollider.h"
#include "../../../Utils/Math/Vec3.h"
#include "../../../Utils/CollisionDetection.h"
#include "ActiveBoxCollider.h"

namespace GAME_NAME::Components::Physics::Collision
{
	using namespace MathUtils;

	void RotationalBoxCollider::CheckStaticCollisionShapes(StaticCollider* collider)
	{
		if (instanceof<StaticBoxCollider>(collider))
		{
			Vec2 push = Vec2::Zero;
			if (Utils::CollisionDetection::BoxWithinBox(m_object->GetPosition(), m_object->GetScale(), collider->GetObject()->GetPosition(), collider->GetObject()->GetScale(), push))
			{
				//Check if the static collider has anything that needs to happen when it collides.
				if (collider->OnCollision != nullptr)
				{
					if (!(collider->OnCollision)(push, collider->GetObject(), m_object))
					{
						return;
					}
				}

				//Push the objects apart.
				m_object->Translate(push);

				//Check if the active collider has anything that needs to happen when it collides.
				if (m_onCollision != nullptr)
				{
					(m_onCollision)(push, m_object, collider->GetObject());
				}
			}
		}
	}

	void RotationalBoxCollider::CheckActiveCollisionShapes(ActiveCollider* collider)
	{
		return;
		if (instanceof<ActiveBoxCollider>(collider))
		{
			Vec2 push = Vec2::Zero;
			if (Utils::CollisionDetection::BoxWithinBox(m_object->GetPosition(), m_object->GetScale(), collider->GetObject()->GetPosition(), collider->GetObject()->GetScale(), push))
			{

				//Push the objects apart.
				m_object->Translate(push);	

				//Check if the active collider has anything that needs to happen when it collides.
				if (m_onCollision != nullptr)
				{
					(m_onCollision)(push, m_object, collider->GetObject());
				}
			}
		}
	}

	bool RotationalBoxCollider::GetCollisionEdge(float xPoint)
	{
		

		return false;
	}
}

