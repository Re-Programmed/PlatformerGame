#include "GlitchedRegion.h"
#include "../../../../Utils/CollisionDetection.h"

#define DEBUG_RENDER_GLITCHED_AREAS false

//Effect parameters, make controllable by the game later...
#define GLITCH_DEPTH 2
#define ADJUST_WIDTH_HEIGHT true

namespace GAME_NAME::Objects
{
	int GlitchedRegion::m_spriteCountBeforePixelated = -1;

	std::vector<GlitchedRegion*> GlitchedRegion::m_regions;

	GlitchedRegion::GlitchedRegion(Vec2 position, Vec2 scale)
#if DEBUG_RENDER_GLITCHED_AREAS
		: GameObject(position, scale,  Renderer::GetSprite(SpriteBase(101)))
#else
		: GameObject(position, scale, nullptr)
#endif
	{
		m_regions.emplace_back(this);
	}


	void GlitchedRegion::DynamicLoadGlitchedArea()
	{
		checkGlitchSprites();

		//Find all the objects in the region that is glitched and update their textures to be the new, lower quality versions.
		for (auto obj : Renderer::GetAllChunkObjectsInArea(m_position, m_scale))
		{
			const int spriteIndex = obj->GetSprite()->GetSpriteId();
			const int spriteCount = Renderer::GetImageCount();
			if (spriteIndex + m_spriteCountBeforePixelated >= spriteCount) {
				continue;
			}

			obj->SetSpriteId(spriteIndex + m_spriteCountBeforePixelated);
		}

		constexpr float ParticleSpeedOut = 0.23f;
		constexpr float ParticleSpeedVariation = 0.1f;

		Particles::Particle particle = Particles::Particle(Vec2{ 0.f }, Vec2{ 3.f }, 0.f, Vec2{ -ParticleSpeedOut, -ParticleSpeedVariation }, 0.f, 0.5f, Renderer::GetSprite(SpriteBase(267) /*Particle sprite.*/));
		particle.TargetOpacity = 0;

		m_particles[0] = new Particles::ParticleEmitter(m_position);
		m_particles[0]->RegisterParticle(particle);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_particles[0], false, 1, false));
		m_particles[0]->SpawnParticlesLooping(0.1, 5, Vec2{ ParticleSpeedOut, ParticleSpeedVariation * 2.f }, 0.f, 10.f, Vec2{ 1.f, m_scale.Y });

		m_particles[1] = new Particles::ParticleEmitter(m_position + Vec2{ 0.f, m_scale.Y });
		particle.Velocity = Vec2{ -ParticleSpeedVariation, 0.f };
		m_particles[1]->RegisterParticle(particle);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_particles[1], false, 1, false));
		m_particles[1]->SpawnParticlesLooping(0.1, 5, Vec2{ ParticleSpeedVariation * 2.f, ParticleSpeedOut/2.22f }, 0.f, 10.f, Vec2{ m_scale.X, 1.f });

		m_particles[2] = new Particles::ParticleEmitter(m_position + Vec2{ m_scale.X, 0.f });
		particle.Velocity = Vec2{ ParticleSpeedOut/2.f, -ParticleSpeedVariation };
		m_particles[2]->RegisterParticle(particle);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_particles[2], false, 1, false));
		m_particles[2]->SpawnParticlesLooping(0.1, 5, Vec2{ ParticleSpeedOut, ParticleSpeedVariation * 2.f }, 0.f, 10.f, Vec2{ 1.f, m_scale.Y });

		m_particles[3] = new Particles::ParticleEmitter(m_position);
		particle.Velocity = Vec2{ -ParticleSpeedVariation, -ParticleSpeedOut };
		m_particles[3]->RegisterParticle(particle);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_particles[3], false, 1, false));
		m_particles[3]->SpawnParticlesLooping(0.1, 5, Vec2{ ParticleSpeedVariation * 2.f, ParticleSpeedOut/2.22f }, 0.f, 10.f, Vec2{ m_scale.X, 1.f });
	}

	void GlitchedRegion::LoadGlitchedAreas()
	{
		checkGlitchSprites();

		for (GlitchedRegion* region : m_regions)
		{
			region->DynamicLoadGlitchedArea();
		}
	}

	void GlitchedRegion::Update(GLFWwindow* window)
	{
		//Find and remove any custom glitchable objects that were in the region and left.
		for (auto pair : m_currentlyActiveGlitchables)
		{
			if (!Utils::CollisionDetection::BoxWithinBox(pair.first->GetPosition(), pair.first->GetScale(), m_position, m_scale))
			{
				pair.second->SetGlitched(false);
				m_currentlyActiveGlitchables.erase(pair.first);
				break;
			}
		}

		//Iterate through active objects that could move in and out of the glitched region.
		for (GameObject* obj : Renderer::GetAllActiveObjectsInArea(m_position, m_scale))
		{
			//Uh oh...
			if (obj->GetSprite() == nullptr)
			{
				continue;
			}

			//Is this object a glitchable?
			Environment::Effects::GlitchableObject* go = dynamic_cast<Environment::Effects::GlitchableObject*>(obj);
			if (go != nullptr)
			{
				if (!go->GetGlitched())
				{
					m_currentlyActiveGlitchables.emplace(obj, go);
					go->SetGlitched(true);
				}

				//Don't update the texture of a glitchable, they will use a custom texture.
				continue;
			}

			//Use lower quality texture for this object.
			const int spriteIndex = obj->GetSprite()->GetSpriteId();

			if (m_spriteCountBeforePixelated > spriteIndex)
			{
				obj->SetSprite(new Sprite(spriteIndex + m_spriteCountBeforePixelated));
			}

		}
	}

	void GlitchedRegion::Render(const Vec2& cameraPosition)
	{
#if DEBUG_RENDER_GLITCHED_AREAS
		//Render this if we are debugging.
		GameObject::Render(cameraPosition);
#endif

		// Don't render this.
	}

	void GlitchedRegion::checkGlitchSprites()
	{
		//Convert all objects within to "8-bit."
		if (m_spriteCountBeforePixelated < 0)
		{
			//Establish initial sprite count to get the offset that all sprites will have after generating lower quality textures.
			const unsigned int& spriteCount = Renderer::GetImageCount() - 1;
			m_spriteCountBeforePixelated = spriteCount;

			//Loop over all sprites.
			for (int i = 1; i <= spriteCount; i++)
			{
				Renderer::TextureData data = Renderer::GetTextureData(i, 0);

				//**Modify the texture data.**

				//Allocate space for the new texture.
				unsigned char* newData = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * (data.Width * data.Height * 4)));

				//Iterate over every pixel, skipping every other pixel (or based on GLITCH_DEPTH)
				int newPixel = 0;
				for (int pixel = 0; pixel < data.Width * data.Height * 4; pixel += 4 * GLITCH_DEPTH /*Skip the columns.*/)
				{
					newData[newPixel++] = data.Data[pixel];
					newData[newPixel++] = data.Data[pixel + 1];
					newData[newPixel++] = data.Data[pixel + 2];
					newData[newPixel++] = data.Data[pixel + 3];

					//Skip the row.
					if ((pixel + (4 * GLITCH_DEPTH)) % (data.Width * 4) == 0)
					{
						pixel += data.Width * (GLITCH_DEPTH - 1) * 4;
					}
				}

				//End modify the texture data.

				Renderer::LoadSpriteFromData(newData, data.Width / (ADJUST_WIDTH_HEIGHT ? GLITCH_DEPTH : 2), data.Height / (ADJUST_WIDTH_HEIGHT ? GLITCH_DEPTH : 2));
				//free(data.Data); We still need it to load other sprites...
				//free(newData);
			}
		}
	}

}


