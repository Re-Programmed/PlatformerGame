#pragma once

#include "../../../../Objects/GameObject.h"
#include "../../../../Rendering/Renderers/Renderer.h"

#define FLASH_EFFECT_SPRITE SpriteBase(131)

namespace GAME_NAME::Objects::Environment::Effects
{
	class FlashEffect
		: public GameObject
	{
	public:
		FlashEffect(Vec2 position, Vec2 scale, double duration, Vec4 color = Vec4{ 1, 1, 1, 1 })
			: GameObject(position, scale, Renderer::GetSprite(FLASH_EFFECT_SPRITE)), m_color(color), m_lifetimeMax(duration)
		{

		}

		void Update(GLFWwindow* window) override;
		void Render(const Vec2& cameraPosition) override;

		/// <summary>
		/// Should be used to create a flash effect.
		/// </summary>
		/// <param name="position">The center of the flash effect.</param>
		/// <param name="scale">The radius of the flash.</param>
		/// <param name="duration">How long the flash remains there.</param>
		/// <param name="color">The color of the flash (rgb out of 1).</param>
		static void CreateFlash(Vec2 center, float scale, double duration, Vec3 color = Vec3{ 1, 1, 1 });

	private:
		const Vec4 m_color;
		const double m_lifetimeMax;
		double m_lifetime = 0.0;
	};
}