#pragma once
#include "../../../../Objects/Particles/ParticleEmitter.h"
#include <cstdarg>

namespace GAME_NAME::Objects::Environment::Effects
{
using namespace Particles;

	/// <summary>
	/// A particle emitter that is created, emits an explosion effect with damage and pushing velocity, and then deletes itself after the explosion ends.
	/// </summary>
	class Explosion
		: public ParticleEmitter
	{
	public:
		/// <summary>
		/// Create an explosion.
		/// </summary>
		/// <param name="position"> - where the explosion is centered.</param>
		/// <param name="radius"> - how far the explosion reaches.</param>
		/// <param name="power"> - velocity and damage scaling for the explosion.</param>
		/// <param name="..."> - accepts any number of spriteIDs for explosion sprites.</param>
		Explosion(Vec2 position, float radius, float power, uint8_t spriteCount = 0, ...);

		/// <summary>
		/// Just the particles.
		/// </summary>
		/// <param name="position"></param>
		/// <param name="radius"></param>
		Explosion(Vec2 position, float radius);

		~Explosion();

		void Update(GLFWwindow* window) override;
	protected:
		void OnCreateParticle(Particle& particle) override;

	private:
		const Vec2 calculateExplosionVelocity(Vec2 explodee);

		const float calculateExplosionDamage(Vec2 explodee);

		const float m_radius, m_power;
	};
}