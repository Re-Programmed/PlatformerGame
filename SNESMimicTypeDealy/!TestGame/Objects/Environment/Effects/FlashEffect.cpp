#include "FlashEffect.h"

#include "../../../../Utils/Time/GameTime.h"

namespace GAME_NAME::Objects::Environment::Effects
{
	void FlashEffect::Update(GLFWwindow* window)
	{
		m_lifetime += Utils::Time::GameTime::DeltaTime::GetDeltaTime();

		if (m_lifetime >= m_lifetimeMax)
		{
			Renderer::DestroyActiveObject(this);
		}
	}

	void FlashEffect::Render(const Vec2& cameraPosition)
	{
		DynamicSprite coloredSprite(m_sprite->GetSpriteId());

		Vec4 color = m_color;
		color.W = 1.f - std::abs(static_cast<float>(m_lifetime * 2.0 / m_lifetimeMax) - 1.f);

		Vec4 colors[4] = {
			color,
			color,
			color,
			color
		};

		coloredSprite.UpdateTextureColor(colors);

		Vec2 renderScale = this->GetScale();
		if (m_lifetime < m_lifetimeMax / 5.0)
		{
			float scalar = static_cast<float>(m_lifetime / (m_lifetimeMax / 5.0));
			renderScale = Vec2{ renderScale.X * scalar, renderScale.Y * scalar };
		}

		if (m_lifetime > 4.0 * m_lifetimeMax / 5.0)
		{
			float scalar = static_cast<float>((m_lifetime - (4.0/5.0 * m_lifetimeMax)) / (m_lifetimeMax / 5.0));
			renderScale = Vec2{ renderScale.X * scalar, renderScale.Y * scalar };
		}

		Vec2 renderPosition = (this->GetPosition() + this->GetScale() / 2.f) - renderScale / 2.f;
		coloredSprite.Render(cameraPosition, renderPosition, renderScale);
	}

	void FlashEffect::CreateFlash(Vec2 center, float scale, double duration, Vec3 color)
	{
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(new FlashEffect(center - Vec2{ scale / 2.f }, Vec2{ scale }, duration, Vec4{ color.X, color.Y, color.Z, 1 }), true, 2, true));
	}


}
