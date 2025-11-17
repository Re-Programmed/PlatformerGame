#pragma once
#include "../GameObject.h"
#include "../../Rendering/Sprite.h"
#include <vector>
#include <algorithm>
#include "../../Components/Physics/Collision/ActiveBoxCollider.h"


namespace GAME_NAME::Objects::Particles
{
	using namespace Rendering;

	struct Particle
	{
		Vec2 Position;
		Vec2 Scale, TargetScale;
		float Rotation;

		Vec2 Velocity, ConstantVelocity;
		float RotationalVelocity;

		float Gravity;

		float Opacity, TargetOpacity;

		std::shared_ptr<Sprite> PSprite;

		float Lifetime;

		Particle(Vec2 position, Vec2 scale, float rotation, Vec2 velocity, float rotationalVelocity, float opacity, Sprite* const sprite, float InitialLifetime = 0.f) : Position(position), Scale(scale), Rotation(rotation), Velocity(velocity),
			RotationalVelocity(rotationalVelocity), Opacity(opacity), PSprite(sprite), Lifetime(InitialLifetime), ConstantVelocity(0), TargetOpacity(opacity), TargetScale(scale), Gravity(0.f) {};

		Particle(Vec2 position, Vec2 scale, float rotation, Vec2 velocity, float rotationalVelocity, float opacity, const std::shared_ptr<Sprite> sprite, float InitialLifetime = 0.f) : Position(position), Scale(scale), Rotation(rotation), Velocity(velocity),
			RotationalVelocity(rotationalVelocity), Opacity(opacity), PSprite(sprite), Lifetime(InitialLifetime), ConstantVelocity(0), TargetOpacity(opacity), TargetScale(scale), Gravity(0.f) {};

		Particle(GameObject* gameObject) : Position(gameObject->GetPosition()), Scale(gameObject->GetScale()), Rotation(gameObject->GetRotation()), Velocity(Vec2::Zero), RotationalVelocity(0.f), Opacity(1.f), PSprite(gameObject->GetSprite()),
			Lifetime(0.f), ConstantVelocity(0), TargetOpacity(1.f), TargetScale(Scale), Gravity(0.f) {};

		Particle(GameObject&& gameObject) : Position(gameObject.GetPosition()), Scale(gameObject.GetScale()), Rotation(gameObject.GetRotation()), Velocity(Vec2::Zero), RotationalVelocity(0.f), Opacity(1.f), PSprite(gameObject.GetSprite()),
			Lifetime(0.f), ConstantVelocity(0), TargetOpacity(1.f), TargetScale(Scale), Gravity(0.f)
		{	
		};

		//Copy operator.
		Particle& operator= (const Particle& other)
		{
			this->ConstantVelocity = other.ConstantVelocity;
			this->Gravity = other.Gravity;
			this->Lifetime = other.Lifetime;
			this->Opacity = other.Opacity;
			this->Position = other.Position;
			this->PSprite = other.PSprite;
			this->Rotation = other.Rotation;
			this->RotationalVelocity = other.RotationalVelocity;
			this->Scale = other.Scale;
			this->TargetOpacity = other.TargetOpacity;
			this->TargetScale = other.TargetScale;
			this->Velocity = other.Velocity;

			return *this;
		}
	};

	class ParticleEmitter 
		: public GameObject
	{
	public:
		ParticleEmitter(Vec2 position, float maxParticleLifetime = 5.f, bool allowCollision = false);

		void Render(const Vec2& cameraPosition) override;

		void SpawnParticle(Vec2 velocity = { 0, 0 }, float gravity = 0.f, float rotation = 0.f, Vec2 offset = { 0, 0 });

		inline void SpawnParticles(uint8_t numParticles, Vec2 maxVelocity = { 0, 0 }, float gravity = 0.f, float rotation = 0.f, Vec2 posVariation = { 0, 0 }, bool destroyOnFinish = false)
		{
			m_destroyOnFinish = destroyOnFinish;

			if (m_allowCollisions)
			{
				//m_previousFrameColliders.rehash(numParticles);
			}


			for (uint8_t i = 0; i < numParticles; i++)
			{
				Vec2 velocity = { (float)(std::rand() * maxVelocity.X * 2) / (float)RAND_MAX - maxVelocity.X, (float)(std::rand() * maxVelocity.Y) / (float)RAND_MAX };
				this->SpawnParticle(velocity, gravity, rotation, { (float)(std::rand() * posVariation.X / (float)RAND_MAX), (float)(std::rand() * posVariation.Y) / (float)RAND_MAX });
			}
		}

		void SpawnParticlesLooping(double interval, uint8_t numParticles, Vec2 maxVelocity = { 0.f, 0.f }, float gravity = 0.f, float rotation = 0.f, Vec2 posVariation = { 0, 0 });
		inline void StopLooping()
		{
			m_loop.pl_LoopInterval = -1.0;
		}

		inline void SetAllowCollisions(bool allowCollisions) { m_allowCollisions = allowCollisions; }

		inline void RegisterParticle(Particle p)
		{
			m_particleCopy.push_back(p);
		}

		inline const size_t GetParticleCount()
		{
			return m_particleCopy.size();
		}

		inline const size_t GetSpawnedCount()
		{
			return m_spawned.size();
		}

		void Update(GLFWwindow* window) override;

		inline void SetOnParticleCollision(std::function<void(Particle& particle)> pFun)
		{
			m_onParticleCollision = pFun;
		}

		inline void SetForcedSpawnVelocity(Vec2 velocity)
		{
			m_forcedSpawnVelocity = velocity;
		}

	protected:
		virtual void OnCreateParticle(Particle& particle)
		{

		}
	private:
		bool m_destroyOnFinish = false;

		std::function<void(Particle& particle)> m_onParticleCollision;

		float m_maxParticleLifetime;

		/// <summary>
		/// Used to determine at what interval particles should be spawned and when the next particle batch should appear.
		/// </summary>
		double m_loopTimer;

		/// <summary>
		/// Conditions set for a particle loop sequence.
		/// </summary>
		struct {
			double pl_LoopInterval = -1.0; //The total time between particles spawning. Less than 0 if the ParticleEmitter is not looping.
			uint8_t pl_NumParticles = 0;
			Vec2 pl_MaxVelocity = Vec2::Zero;
			float pl_Gravity = 0.f;
			float pl_Rotation = 0.f;

			Vec2 pl_posVariation = Vec2::Zero;
		} m_loop;

		std::vector<Particle> m_particleCopy;

		std::vector<Particle> m_spawned;

		/// <summary>
		/// If true will add active box colliders to particles.
		/// </summary>
		bool m_allowCollisions;
		/// <summary>
		/// List of GameObjects that represent particles for collision.
		/// </summary>
		std::vector<Components::Physics::Collision::ActiveBoxCollider*> m_previousFrameColliders;

		Vec2 m_forcedSpawnVelocity;
	};
}