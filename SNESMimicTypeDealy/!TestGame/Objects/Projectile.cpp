#include "Projectile.h"

#include "../../Rendering/Renderers/Renderer.h"

#include "../../Utils/Time/GameTime.h"

#include "./Environment/Effects/Explosion.h"

#define EXPLOSIVE_PROJECTILE_FUSE_MAX 0.33

namespace GAME_NAME::Objects
{
	constexpr float DefaultProjectileGravity = 5.5F;
	constexpr float ProjectileRangeScale = 13.f;

	std::array<const Projectile::ProjectileType, PROJECTILE_TYPE_COUNT> Projectile::m_projectileTypes = { ProjectileType(Vec2{ 10, 10 }, SpriteBase(381)) };

	Projectile::Projectile(Vec2 position, int damage, float range, uint8_t projectileType, bool directionLeft)
		: ActiveBoxCollisionGravityObject(position, m_projectileTypes[projectileType].Scale, Renderer::GetSprite(m_projectileTypes[projectileType].SpriteID)),
		m_range(range * ProjectileRangeScale), m_projectileType(projectileType), m_damage(damage)
	{
		this->m_physics->SetGravityStrength(DefaultProjectileGravity);

		range *= ProjectileRangeScale;

		this->m_physics->SetVelocity({ range * (directionLeft ? -1.f : 1.f), range / 1.5f});
		this->m_physics->SetFrictionDrag(0.f);
		this->m_physics->SetRotationalVelocity(2.f);
	}

	void Projectile::Update(GLFWwindow* window)
	{
		if (m_fuse > 0.0)
		{
			m_fuse += Utils::Time::GameTime::GetScaledDeltaTime();
		}

		ActiveBoxCollisionGravityObject::Update(window);
	}

	void Projectile::onCollision(Vec2 push, Objects::GameObject* gameObject)
	{
using namespace Environment::Effects;

		//Explosive
		if (m_projectileType == 0)
		{
			m_fuse += 0.01;
			this->m_physics->SetVelocity(this->m_physics->GetVelocity() / 2.f);

			if (m_fuse > EXPLOSIVE_PROJECTILE_FUSE_MAX)
			{
				Explosion* explosion = new Explosion(m_position, m_damage, m_damage);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(explosion, true, 1, 1));

				Renderer::DestroyActiveObject(this);
			}
		}
	}
}
