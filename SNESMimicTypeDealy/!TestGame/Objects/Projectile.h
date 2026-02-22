#pragma once
#include "../../Components/Physics/Collision/Helpers/ActiveBoxCollisionGravityObject.h"
#include <array>

namespace GAME_NAME::Objects
{
	using namespace GAME_NAME::Components::Physics::Collision;


	/// <summary>
	/// Should be spawned as an active object.
	/// </summary>
	class Projectile
		: public ActiveBoxCollisionGravityObject
	{
	public:
		Projectile(Vec2 position, int damage, float range, uint8_t projectileType, bool directionLeft = false);

		void Update(GLFWwindow* window) override;

		void onCollision(Vec2 push, Objects::GameObject* gameObject) override;
	private:
		const int m_damage; const float m_range; const uint8_t m_projectileType;

		double m_fuse = 0.0;

#define PROJECTILE_TYPE_COUNT 1
		struct ProjectileType
		{
			Vec2 Scale;
			int SpriteID;
		};

		static std::array<const ProjectileType, PROJECTILE_TYPE_COUNT> m_projectileTypes;
	};
}