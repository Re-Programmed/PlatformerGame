#include "InnerUpperWall.h"

#include "../../../TestGame.h"

namespace GAME_NAME::Objects::Environment::Buildings
{
	InnerUpperWall::InnerUpperWall(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float transitionOffset, float transitionMaxOffset)
		: GameObject(position, scale, sprite), m_transitionOffset(transitionOffset), m_transitionMaxOffset(transitionMaxOffset)
	{

	}

	void InnerUpperWall::Render(const Vec2& cameraPos)
	{
		const float playerY = TestGame::ThePlayer->GetPosition().Y;

		if (playerY < m_position.Y)
		{
			GameObject::Render(cameraPos);
			return;
		}

		float max = m_scale.Y + m_transitionOffset;

		if (playerY > m_position.Y + m_transitionMaxOffset && playerY < m_position.Y + max)
		{
			float prop = 1.f - ((playerY - m_position.Y) / max);

			DynamicSprite transparentSprite(m_sprite->GetSpriteId());

			const Vec4 col{ 1.f, 1.f, 1.f, prop };
			const Vec4 colors[4] = { col, col, col, col };

			transparentSprite.UpdateTextureColor(colors);
			transparentSprite.Render(cameraPos, m_position + m_scale, Vec2{ -m_scale.X, -m_scale.Y }, m_rotation);

			return;
		}

		//Don't render since the player is above the wall.
	}
}

