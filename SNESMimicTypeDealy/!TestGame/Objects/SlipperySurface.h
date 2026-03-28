#pragma once

#include "../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"

namespace GAME_NAME::Objects
{
using namespace GAME_NAME::Components::Physics::Collision;

	class SlipperySurface
		: public StaticBoxCollisionObject
	{
	public:
		SlipperySurface(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float slipFactor = 1.f);


	private:
		static bool onCollision(GAME_NAME::MathUtils::Vec2 push, Objects::GameObject* self, Objects::GameObject* other);

		const float m_slipFactor;
	};
}