#include "Explosion.h"

#include "../../../../Rendering/Renderers/Renderer.h"
#include "../../../Objects/Enemies/Enemy.h"
#include "../../../TestGame.h"

#include "./ExplosionEffectedObject.h"

#define EXPLOSION_VELOCITY_CONSTANT 250.f
#define PARTICLE_VELOCITY_DAMP 0.002f
#define MAX_PARTICLE_VELOCITY 100.f

#define MAX_EXPLOSION_LAUNCH_VELOCITY 525.f

#define EXPLOSION_DAMAGE_CONSTANT 10

#define DEFAULT_EXPLOSION_SPRITES_COUNT 8
/// <summary>
/// Sprites to be used for explosions if none are provided.
/// </summary>
constexpr int DEFAULT_EXPLOSION_SPRITES[DEFAULT_EXPLOSION_SPRITES_COUNT] = { SpriteBase(238), SpriteBase(239), SpriteBase(240), SpriteBase(241), SpriteBase(242), SpriteBase(243), SpriteBase(244), SpriteBase(245) };

namespace GAME_NAME::Objects::Environment::Effects
{
	Explosion::Explosion(Vec2 position, float radius, float power, uint8_t spriteCount, ...)
		: ParticleEmitter(position, 1.f), m_radius(radius), m_power(power)
	{
		if (spriteCount > 0)
		{
			va_list args;
			va_start(args, spriteCount);

			//Create all the particles.
			for (int i = 0; i < spriteCount; i++)
			{
				Particle p(Vec2::Zero, 1.f, (std::rand() * 60 - 30) / RAND_MAX, Vec2::Zero, 0.f, 100.f, Renderer::GetSprite(va_arg(args, int)));
				this->RegisterParticle(p);
			}

			va_end(args);
		}
		else {
			//Create all the particles.
			for (int i = 0; i < DEFAULT_EXPLOSION_SPRITES_COUNT; i++)
			{
				Particle p(Vec2::Zero, 1.f, (std::rand() * 60 - 30) / RAND_MAX, Vec2::Zero, 0.f, 100.f, Renderer::GetSprite(DEFAULT_EXPLOSION_SPRITES[i]));
				this->RegisterParticle(p);
			}
		}

		//Adjust position so that particles spawn around the center of the emitter.
		m_position -= radius * 0.5f;
		this->SpawnParticles(std::floor(power * 6.f), Vec2{ 0 }, 0.f, (std::rand() * 180) / RAND_MAX, Vec2{ radius });
		m_position += radius * 0.5f;

		const Vec2 playerPos = TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale()/2.f;
		if (Vec2::Distance(playerPos, m_position) < radius)
		{
			Vec2 vel = calculateExplosionVelocity(playerPos);

			if (vel.X > MAX_EXPLOSION_LAUNCH_VELOCITY) { vel.X = MAX_EXPLOSION_LAUNCH_VELOCITY; }
			if (vel.X < -MAX_EXPLOSION_LAUNCH_VELOCITY) { vel.X = -MAX_EXPLOSION_LAUNCH_VELOCITY; }
			if (vel.Y > MAX_EXPLOSION_LAUNCH_VELOCITY) { vel.Y = MAX_EXPLOSION_LAUNCH_VELOCITY; }
			if (vel.Y < -MAX_EXPLOSION_LAUNCH_VELOCITY) { vel.Y = -MAX_EXPLOSION_LAUNCH_VELOCITY; }

			TestGame::ThePlayer->Dive(vel, calculateExplosionDamage(playerPos));
			//TestGame::ThePlayer->Damage(calculateExplosionDamage(playerPos), this);
		}

		for (GameObject* activeObject : Renderer::GetAllActiveObjectsInArea(this->m_position - Vec2{ radius }, radius * 2.f))
		{
			Enemies::Enemy* enemy = dynamic_cast<Enemies::Enemy*>(activeObject);
			if (enemy != nullptr)
			{
				enemy->AddVelocity(calculateExplosionVelocity(enemy->GetPosition() + enemy->GetScale()/2.f));

				enemy->AddRotationalVelocity((std::rand() * 2) / RAND_MAX < 1 ? power : -power);
			}

			ExplosionEffectedObject* eeo = dynamic_cast<ExplosionEffectedObject*>(activeObject);
			if (eeo)
			{
				eeo->Exploded(m_position, m_power);
			}
		}

		for (GameObject* staticObject : Renderer::GetAllChunkObjectsInArea(this->m_position - Vec2{ radius }, radius * 2.f))
		{
			ExplosionEffectedObject* eeo = dynamic_cast<ExplosionEffectedObject*>(staticObject);
			if (eeo)
			{
				eeo->Exploded(m_position, m_power);
			}
		}
	}


	Explosion::~Explosion()
	{
		ParticleEmitter::~ParticleEmitter();
	}

	void Explosion::Update(GLFWwindow* window)
	{
		ParticleEmitter::Update(window);

		//All particles are gone.
		if (GetSpawnedCount() < 1)
		{
			Renderer::DestroyActiveObject(this); //Also deletes the object...
		}
	}

	void Explosion::OnCreateParticle(Particle& particle)
	{
		particle.Scale = (std::rand() * m_radius / 2) / RAND_MAX;

		particle.Velocity = Vec2::Zero;
		particle.ConstantVelocity = calculateExplosionVelocity(particle.Position - m_radius * 0.5f);
		particle.ConstantVelocity = Vec2{ particle.ConstantVelocity.X * PARTICLE_VELOCITY_DAMP , particle.ConstantVelocity.Y * PARTICLE_VELOCITY_DAMP };

		if (particle.ConstantVelocity.X > MAX_PARTICLE_VELOCITY)
		{
			particle.ConstantVelocity.X = MAX_PARTICLE_VELOCITY / 2.f;
		}else if (particle.ConstantVelocity.X < -MAX_PARTICLE_VELOCITY)
		{
			particle.ConstantVelocity.X = -MAX_PARTICLE_VELOCITY / 2.f;
		}
		
		if (particle.ConstantVelocity.Y > MAX_PARTICLE_VELOCITY)
		{
			particle.ConstantVelocity.Y = MAX_PARTICLE_VELOCITY / 2.f;
		}
		else if (particle.ConstantVelocity.Y < -MAX_PARTICLE_VELOCITY)
		{
			particle.ConstantVelocity.Y = -MAX_PARTICLE_VELOCITY / 2.f;
		}

		particle.Opacity = 1;

		particle.TargetOpacity = 0;
		particle.TargetScale = 0;
	}

	const Vec2 Explosion::calculateExplosionVelocity(Vec2 explodee)
	{
		const float dist = Vec2::Distance(explodee, m_position);
		const float invDistSq = 1.f / (dist * dist);
		const Vec2 explosionVelocity(invDistSq * EXPLOSION_VELOCITY_CONSTANT * m_power * (explodee.X - m_position.X), invDistSq * EXPLOSION_VELOCITY_CONSTANT * m_power * (explodee.Y - m_position.Y));

		return explosionVelocity;
	}

	const float Explosion::calculateExplosionDamage(Vec2 explodee)
	{
		const float dist = Vec2::Distance(explodee, m_position);
		const float invDistRoot = std::sqrtf(m_radius / dist);
		return invDistRoot * EXPLOSION_DAMAGE_CONSTANT + (m_power/2.5f);
	}
}