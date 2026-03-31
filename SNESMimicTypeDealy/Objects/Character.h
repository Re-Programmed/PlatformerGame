#pragma once
#include "../Components/Physics/Collision/Helpers/ActiveBoxCollisionGravityObject.h"

namespace GAME_NAME::Objects
{
using namespace GAME_NAME::Components::Physics::Collision;

constexpr float DefaultCharacterGravity = 5.5f;

	class Character
		: public ActiveBoxCollisionGravityObject
	{
	public:
		Character(Vec2 position, Vec2 scale, bool gravity)
			: ActiveBoxCollisionGravityObject(position, scale, nullptr)
		{
			m_physics->SetGravityStrength(gravity ? DefaultCharacterGravity : 0.f);
		}
	};
}