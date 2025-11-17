#pragma once

#include "../../../Objects/GameObject.h"
#include "../../../Components/Animation/AnimatorComponent.h"
#include "../../../Rendering/DynamicSprite.h"
#include "../../../Rendering/Renderers/Renderer.h"

namespace GAME_NAME::Objects::Environment
{
	class Waterfall
		: public GameObject
	{

	public:
		Waterfall(Vec2 position, Vec2 scale, Rendering::Sprite* sprite1, Rendering::Sprite* sprite2, Rendering::Sprite* sprite3, int segments = 1, bool isBaseSegment = true)
			: GameObject(position, scale, new Sprite(sprite1->GetSpriteId())), m_segments(segments), m_isBaseSegment(isBaseSegment)
		{
			std::vector<std::shared_ptr<Sprite>> sprites;
			sprites.emplace_back(sprite1);
			sprites.emplace_back(sprite2);
			sprites.emplace_back(sprite3);
			std::shared_ptr<GAME_NAME::Components::Animation::Animation> waterAnimation = std::make_shared<GAME_NAME::Components::Animation::Animation>(Components::Animation::AnimData(sprites), ANIM_6_SPF);

			std::vector<std::shared_ptr<Components::Animation::Animation>> anims{ waterAnimation };

			m_animator.reset(new Components::Animation::AnimatorComponent(anims));
			m_animator->SetCurrentAnimation(0);

			for (int i = 1; i < segments; i++)
			{
				Waterfall* segment = new Waterfall(Vec2{ m_position.X, m_position.Y + m_scale.Y * i }, m_scale, sprite1, sprite2, sprite3, 1, false);
				Renderer::LoadObject(segment);
			}
		}

		void Render(const Vec2& cameraPos) override;
		void Update(GLFWwindow* window) override;

	private:
		const int m_segments;
		const bool m_isBaseSegment;
		
		double m_boudlerSpawnTimer = 0.0;
		double m_boulderSpawnRate = 0.004;

		std::unique_ptr<Components::Animation::AnimatorComponent> m_animator;
	};
}