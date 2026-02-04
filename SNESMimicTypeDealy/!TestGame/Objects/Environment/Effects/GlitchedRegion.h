#pragma once

#include "../../../../Objects/GameObject.h"
#include "../../../../Rendering/Renderers/Renderer.h"
#include "GlitchableObject.h"

#include "../../../../Objects/Particles/ParticleEmitter.h"

#include <map>

namespace GAME_NAME::Objects
{
	class GlitchedRegion
		: public GameObject
	{
	public:

		GlitchedRegion(Vec2 position, Vec2 scale);

		/// <summary>
		/// Should be called if a glitched area is created after a level has already loaded to load all the glitched textures within itself.
		/// </summary>
		void DynamicLoadGlitchedArea();

		/// <summary>
		/// Creates all the glitchy effects. Should be called after level load.
		/// </summary>
		static void LoadGlitchedAreas();

		void Update(GLFWwindow* window) override;

		void Render(const Vec2& cameraPosition) override;

		/// <summary>
		/// Call this whenever sprites are reloaded in order to recreate pixelated versions.
		/// </summary>
		static void ResetSpriteCount()
		{
			m_spriteCountBeforePixelated = -1;
		}

		~GlitchedRegion()
		{
			Renderer::DestroyObject(m_particles[0]);
			Renderer::DestroyObject(m_particles[1]);
			Renderer::DestroyObject(m_particles[2]);
			Renderer::DestroyObject(m_particles[3]);

			for (int i = 0; i < m_regions.size(); i++)
			{
				if (m_regions[i] == this)
				{
					m_regions.erase(m_regions.begin() + i);
					break;
				}
			}
		}

	private:
		static std::vector<GlitchedRegion*> m_regions;

		static int m_spriteCountBeforePixelated;

		Particles::ParticleEmitter* m_particles[4];

		/// <summary>
		/// Ensures all glitched versions of sprites exist.
		/// </summary>
		static void checkGlitchSprites();

		static inline GLuint getTextureRelativeGlitched(GLuint textureID)
		{
			return textureID + m_spriteCountBeforePixelated;
		};

		std::unordered_map<GameObject*, Environment::Effects::GlitchableObject*> m_currentlyActiveGlitchables;
	};
}