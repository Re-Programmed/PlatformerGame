#include "Projectile.h"

#include "../../Rendering/Renderers/Renderer.h"

#include "../../Utils/Time/GameTime.h"

#include "./Environment/Effects/Explosion.h"
#include "../../Audio/SoundEvents.h"
#include "../Objects/SoundMaterial.h"

#include "../Objects/Environment/Effects/BloodMark.h"

#include "../Objects/SlipperySurface.h"

#define EXPLOSIVE_PROJECTILE_FUSE_MAX 0.33

#define EXPLOSION_MARK_SPRITE SpriteBase(133)
#define EXPLOSION_MARK_SCALE Vec2{24,13.5f}

#define BUTTER_SPLASH_SPRITE SpriteBase(446)
#define BUTTER_SPLASH_WIDTH 32.f
#define BUTTER_SPLASH_HEIGHT 7.f

#define GLUE_SPLASH_SPRITE SpriteBase(481)
#define GLUE_SPLASH_WIDTH 32.f
#define GLUE_SPLASH_HEIGHT 7.f

namespace GAME_NAME::Objects
{
	constexpr float DefaultProjectileGravity = 5.5F;
	constexpr float ProjectileRangeScale = 13.f;

	std::array<const Projectile::ProjectileType, PROJECTILE_TYPE_COUNT> Projectile::m_projectileTypes = { 
		ProjectileType(Vec2{ 10, 10 }, SpriteBase(381)), 
		ProjectileType(Vec2{ 10, 10 }, SpriteBase(383)), 
		ProjectileType(Vec2{ 10, 10 }, SpriteBase(384)),
		ProjectileType(Vec2{ 10, 10 }, SpriteBase(445)) };

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

	void Projectile::onCollision(Vec2 push, Objects::GameObject* gameObject, GameObject* other)
	{
using namespace Environment::Effects;

		//Explosive
		if (m_projectileType <= 2)
		{
			//Play a sound as the projectile has hit something.
			if (m_fuse <= 0)
			{
				Audio::SoundEvents::PlaySoundAtPoint(SoundMaterial::GetHitEffectForMaterial(SoundMaterial::GetSoundMaterial(other)), m_position, 0.5f);
			}

			m_fuse += 0.01;
			this->m_physics->SetVelocity(this->m_physics->GetVelocity() / 2.f);

			if (m_fuse > EXPLOSIVE_PROJECTILE_FUSE_MAX)
			{
				Explosion* explosion = new Explosion(m_position, m_damage, m_damage);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(explosion, true, 1, 1));


				Environment::BloodMark* floorMark = new Environment::BloodMark(this, m_position + Vec2(m_scale.X / 2.f, 0.f), Renderer::GetSprite(EXPLOSION_MARK_SPRITE), EXPLOSION_MARK_SCALE * (m_scale.X/10.f));
				Renderer::LoadObject(floorMark, 1);

				Renderer::DestroyActiveObject(this);
			}
		}
		//Butter.
		else if (m_projectileType <= 3)
		{
			Vec2 pos(m_position.X - (BUTTER_SPLASH_WIDTH/2.f), other->GetPosition().Y + other->GetScale().Y - BUTTER_SPLASH_HEIGHT + 1.f);

			SlipperySurface* butterSplash = new SlipperySurface(pos, Vec2{ BUTTER_SPLASH_WIDTH, BUTTER_SPLASH_HEIGHT }, Renderer::GetSprite(BUTTER_SPLASH_SPRITE));
			Renderer::InstantiateObject(Renderer::InstantiateGameObject(butterSplash, false, 3, false));

			Renderer::DestroyActiveObject(this);
		}
		//Glue
		else if (m_projectileType <= 4)
		{
			Vec2 pos(m_position.X - (GLUE_SPLASH_WIDTH / 2.f), other->GetPosition().Y + other->GetScale().Y - GLUE_SPLASH_HEIGHT + 1.f);

			SlipperySurface* butterSplash = new SlipperySurface(pos, Vec2{ GLUE_SPLASH_WIDTH, GLUE_SPLASH_HEIGHT }, Renderer::GetSprite(GLUE_SPLASH_SPRITE), -1.f);
			Renderer::InstantiateObject(Renderer::InstantiateGameObject(butterSplash, false, 3, false));

			Renderer::DestroyActiveObject(this);
		}
	}
}
