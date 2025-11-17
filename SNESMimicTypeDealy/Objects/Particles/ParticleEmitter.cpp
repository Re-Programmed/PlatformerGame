#include "ParticleEmitter.h"
#include "../../Utils/Time/GameTime.h"
#include "../../Rendering/DynamicSprite.h"

#include "../../Rendering/Renderers/Renderer.h"

#include "../../Components/Physics/Collision/CollisionManager.h"

namespace GAME_NAME::Objects::Particles
{

	ParticleEmitter::ParticleEmitter(Vec2 position, float maxParticleLifetime, bool allowCollision)
		: GameObject(position, Vec2(0, 0), nullptr, 0.0F), m_maxParticleLifetime(maxParticleLifetime), m_allowCollisions(allowCollision), m_loopTimer(0.0), m_forcedSpawnVelocity(0)
	{
		
	}

	void ParticleEmitter::Render(const Vec2& cameraPosition)
	{
		//Render Particles

		for (int i = 0; i < m_spawned.size(); i++)
		{
			
			DynamicSprite particleSprite = DynamicSprite(m_spawned[i].PSprite->GetSpriteId());
			
			const Vec4 cols[4]{
				{ 1.f, 1.f, 1.f, m_spawned[i].Opacity },
				{ 1.f, 1.f, 1.f, m_spawned[i].Opacity },
				{ 1.f, 1.f, 1.f, m_spawned[i].Opacity },
				{ 1.f, 1.f, 1.f, m_spawned[i].Opacity }
			};

			particleSprite.UpdateTextureColor(cols);

			particleSprite.Render(cameraPosition, m_spawned[i].Position, m_spawned[i].Scale, m_spawned[i].Rotation);
		}
		
	}

	void ParticleEmitter::SpawnParticle(Vec2 velocity, float gravity, float rotation, Vec2 offset)
	{
using namespace GAME_NAME::Components::Physics::Collision;

		int index = std::rand() / (RAND_MAX / m_particleCopy.size());
		if (index >= m_particleCopy.size()) { return; }
		Particle& copy = m_particleCopy[index];
		Particle emit(copy.Position, copy.Scale, copy.Rotation, copy.Velocity, copy.RotationalVelocity, copy.Opacity, copy.PSprite, copy.Lifetime);
		emit.TargetOpacity = copy.TargetOpacity;
		emit.TargetScale = copy.TargetScale;
		emit.ConstantVelocity = copy.ConstantVelocity;

		emit.Position += offset + m_position + Vec2(std::rand() * m_scale.X / RAND_MAX, std::rand() * m_scale.Y / RAND_MAX);
		emit.Velocity += velocity;
		emit.Gravity = gravity;

		if (m_forcedSpawnVelocity.X != 0 || m_forcedSpawnVelocity.Y != 0)
		{
			emit.Velocity = m_forcedSpawnVelocity + velocity;
			emit.ConstantVelocity = 0;
		}

		//emit.Rotation = rotation;

		if (emit.ConstantVelocity.X != 0) {
			//emit.ConstantVelocity.X += ((float)(std::rand()) / (float)RAND_MAX) * 1.75f - 0.375f;
		}

		if (emit.ConstantVelocity.Y != 0) {
			emit.ConstantVelocity.Y += ((float)(std::rand()) / (float)RAND_MAX) * 1.75f - 0.375f;
		}

		OnCreateParticle(emit);
		m_spawned.push_back(emit);

		if (m_allowCollisions)
		{
			GameObject* particleRepresentation = new GameObject(emit.Position, emit.Scale, nullptr, emit.Rotation);
			ActiveBoxCollider* abc = new ActiveBoxCollider(false);
			abc->Init(particleRepresentation);

			m_previousFrameColliders.push_back(abc);
		}
	}

	void ParticleEmitter::SpawnParticlesLooping(double interval, uint8_t numParticles, Vec2 maxVelocity, float gravity, float rotation, Vec2 posVariation)
	{
		m_loopTimer = 0.0;
		m_loop.pl_LoopInterval = interval;

		m_loop.pl_NumParticles = numParticles;
		m_loop.pl_MaxVelocity = maxVelocity;
		m_loop.pl_Gravity = gravity;
		m_loop.pl_Rotation = rotation;
		m_loop.pl_posVariation = posVariation;
	}

	void ParticleEmitter::Update(GLFWwindow* window)
	{
		using namespace GAME_NAME::Components::Physics::Collision;

		for (int i = 0; i < m_spawned.size(); ++i)
		{

			if (m_spawned[i].Lifetime < 0.f || (m_spawned[i].Lifetime >= m_maxParticleLifetime && m_maxParticleLifetime > 0.f))
			{
				m_spawned.erase(m_spawned.begin() + i);

				if (m_allowCollisions)
				{
					delete m_previousFrameColliders[i]->GetObject();
					delete m_previousFrameColliders[i];
					m_previousFrameColliders.erase(m_previousFrameColliders.begin() + i);
				}

				//i++;
				continue;
			}

			//Update lifetime
			if (m_maxParticleLifetime > 0.f)
			{
				m_spawned[i].Lifetime += (float)Utils::Time::GameTime::GetScaledDeltaTime();
			}

			//Update particles based on their colliders.
			if (m_allowCollisions && m_previousFrameColliders.size() > i)
			{
				if (m_onParticleCollision != nullptr)
				{
					//m_spawned[i].Position.X != m_previousFrameColliders[i]->GetObject()->GetPosition().X || m_spawned[i].Position.Y != m_previousFrameColliders[i]->GetObject()->GetPosition().Y
					if (Vec2::Distance(m_spawned[i].Position, m_previousFrameColliders[i]->GetObject()->GetPosition()) > 0.5f)
					{
						m_onParticleCollision(m_spawned[i]);
					}
				}

				m_spawned[i].Position = m_previousFrameColliders[i]->GetObject()->GetPosition();
				CollisionManager::RegisterActiveColliderToBuffer(m_previousFrameColliders[i]);
			}

			//Update velocities.
			m_spawned[i].Position += (m_spawned[i].Velocity + m_spawned[i].ConstantVelocity) * (static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * 60.f);
			m_spawned[i].Rotation += m_spawned[i].RotationalVelocity;

			//Update scale and opacity.
			m_spawned[i].Opacity = std::lerp(m_spawned[i].Opacity, m_spawned[i].TargetOpacity, 1.f / (m_maxParticleLifetime * 30.f));
			m_spawned[i].Scale.X = std::lerp(m_spawned[i].Scale.X, m_spawned[i].TargetScale.X, 1.f / (m_maxParticleLifetime * 30.f));
			m_spawned[i].Scale.Y = std::lerp(m_spawned[i].Scale.Y, m_spawned[i].TargetScale.Y, 1.f / (m_maxParticleLifetime * 30.f));

			//Add drag.
			if (m_spawned[i].Velocity.X >= 0.1f) { m_spawned[i].Velocity.X -= (static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * 60.f) * (float)std::rand() / (RAND_MAX * 100.f); }
			else if (m_spawned[i].Velocity.X <= -0.1f) { m_spawned[i].Velocity.X += (static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * 60.f) * (float)std::rand() / (RAND_MAX * 100.f); }
			else
			{
				m_spawned[i].Velocity.X = 0.f;
			}

			if (m_spawned[i].Velocity.Y > -2.f)
			{
				m_spawned[i].Velocity.Y -= m_spawned[i].Gravity * (static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * 60.f);
			}

			if (m_spawned[i].RotationalVelocity >= 0.1f) { m_spawned[i].RotationalVelocity -= 0.1f * (static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * 60.f); }
			else if (m_spawned[i].RotationalVelocity <= -0.1f) { m_spawned[i].RotationalVelocity += 0.1f * (static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * 60.f); }
			else
			{
				m_spawned[i].RotationalVelocity = 0.f;
			}

			if (m_allowCollisions && m_previousFrameColliders.size() > i)
			{
				m_previousFrameColliders[i]->GetObject()->SetPosition(m_spawned[i].Position);
			}
		}

		//Should we destroy the particle emitter when it's done?
		if (m_destroyOnFinish && m_spawned.size() < 1 && m_loop.pl_LoopInterval <= 0.0)
		{
			m_destroyOnFinish = false;
			Rendering::Renderer::DestroyObject(this);
			return;
		}

		//Handle looping.
		if (m_loop.pl_LoopInterval > 0.0 && m_loopTimer < 0.0)
		{
			//Spawn particles and restart timer.
			SpawnParticles(m_loop.pl_NumParticles, m_loop.pl_MaxVelocity, m_loop.pl_Gravity, m_loop.pl_Rotation, m_loop.pl_posVariation);
			m_loopTimer = m_loop.pl_LoopInterval;
		}
		else {
			//Decrement the timer.
			m_loopTimer -= Utils::Time::GameTime::GetScaledDeltaTime();
		}
	}
}
