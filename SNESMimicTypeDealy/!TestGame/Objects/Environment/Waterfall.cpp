#include "Waterfall.h"
#include "../../../Utils/Time/GameTime.h"

#include "../../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"
#include "../../../Components/Physics/GravityComponent.h"
#include "../../TestGame.h"

#define WATERFALL_BOULDER_TEXTURES { 121, 122, 123 }
#define WATERFALL_BOULDER_ROTATION_SPEED 15.f
#define WATERFALL_BOULDER_SCALE Vec2{ 24.f, 24.f }

#define BOULDER_DAMAGE 10

namespace GAME_NAME::Objects::Environment
{
	class Boulder
		: public Components::Physics::Collision::StaticBoxCollisionObject
	{
	public:
		Boulder(Vec2 position, Vec2 scale, Rendering::Sprite* sprite)
			: StaticBoxCollisionObject(position, scale, sprite)
		{
			m_physics.SetGravityStrength(0.8f);
			SetOnCollision(onCollision);
		}

		void Update(GLFWwindow* window) override
		{
			RotateAboutCenter(Utils::Time::GameTime::GetScaledDeltaTime() * WATERFALL_BOULDER_ROTATION_SPEED);
			m_physics.Update(window, this);

			Components::Physics::Collision::StaticBoxCollisionObject::Update(window);

			if (m_position.Y < -50.f)
			{
				Renderer::DestroyActiveObjectImmediate(this);
			}
		}
		
	private:
		Components::Physics::GravityComponent m_physics;

		static bool onCollision(Vec2 push, GameObject* self, GameObject* other)
		{
			if (other == GAME_NAME::TestGame::ThePlayer.get())
			{
				GAME_NAME::TestGame::ThePlayer->Damage(BOULDER_DAMAGE, self);

				Boulder* selfB = dynamic_cast<Boulder*>(self);
				if (selfB != nullptr)
				{
					selfB->explode();
				}

				return false;
			}

			return true;
		}

		void explode()
		{
			Particles::ParticleEmitter* pe = new Particles::ParticleEmitter(m_position + m_scale/2.f, 3.f, true);

			Particles::Particle boulderParticle(this);
			boulderParticle.Position = Vec2{ 0.f, 0.f };
			boulderParticle.Scale = { 10.f, 10.f };
			boulderParticle.TargetOpacity = 0.f;
			boulderParticle.RotationalVelocity = 1.f;
			boulderParticle.TargetScale = 0.f;
			pe->RegisterParticle(boulderParticle);

			Renderer::LoadObject(pe);
			pe->SpawnParticles(20, { 1.f, 2.f }, 0.3f, 90.f, { 3.f, 3.f }, true);

			Renderer::DestroyActiveObject(this);
		}
	};


	Vec4 Waterfall_Color[4]
	{
		Vec4{ 1.f, 1.f, 1.f, 0.6f },
		Vec4{ 1.f, 1.f, 1.f, 0.6f },
		Vec4{ 1.f, 1.f, 1.f, 0.6f },
		Vec4{ 1.f, 1.f, 1.f, 0.6f }
	};

using namespace GAME_NAME::Rendering;

	void Waterfall::Render(const Vec2& cameraPos)
	{
		DynamicSprite* sprite = new DynamicSprite(this->m_sprite->GetSpriteId());
		sprite->UpdateTextureColor(Waterfall_Color);
		sprite->Render(cameraPos, m_position, m_scale, m_rotation);
		delete sprite;
	}

	void Waterfall::Update(GLFWwindow* window)
	{
		m_animator->Update(window, this);

		if (m_isBaseSegment)
		{
			//Spawn boulders.

			m_boudlerSpawnTimer += Utils::Time::GameTime::DeltaTime::GetDeltaTime();

			if (m_boudlerSpawnTimer >= 0.0166667 /*Call once every frame if at 60fps*/)
			{
				m_boudlerSpawnTimer = 0.0;

				if (std::rand() <= RAND_MAX * m_boulderSpawnRate)
				{
					m_boulderSpawnRate -= 0.0006;

					Vec2 pos(m_position.X + (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)) * m_scale.X, m_position.Y + m_scale.Y * m_segments);
					Vec2 scale = WATERFALL_BOULDER_SCALE * std::clamp((static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)), 0.5, 1.0);
					pos.Y -= scale.Y + 4.f;

					uint16_t spriteIndex = 0;

					for (uint16_t texture : WATERFALL_BOULDER_TEXTURES)
					{
						if (spriteIndex == 0)
						{
							if (std::rand() < RAND_MAX / 3)
							{
								spriteIndex = texture;
							}
						}

					}
					if (spriteIndex == 0) { spriteIndex = 121; }

					Boulder* boulder = new Boulder(pos, scale, Renderer::GetSprite(spriteIndex));
					boulder->SetRotationAboutCenter((static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)) * 180.f);
					Renderer::LoadActiveObject(boulder, 2);
				}
				else {
					m_boulderSpawnRate += Utils::Time::GameTime::DeltaTime::GetDeltaTime() * 0.0004;
				}

			}
		}
	}

}
