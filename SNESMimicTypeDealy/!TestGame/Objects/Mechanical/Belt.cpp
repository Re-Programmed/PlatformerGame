#include "Belt.h"

#include "../../../Rendering/Renderers/Renderer.h"

#define BELT_END_SPRITE SpriteBase(441)
#define BELT_SEGMENT_SPRITE SpriteBase(443)

#define BELT_SEGMENT_WIDTH 6.f

namespace GAME_NAME::Objects::Mechanical
{
	Belt::Belt(Cog* firstConnection, Cog* secondConnection)
		: GameObject(firstConnection->GetPosition() + firstConnection->GetScale() / 2.f, Vec2{ 0.f }, Renderer::GetSprite(BELT_SEGMENT_SPRITE)), m_firstConnection(firstConnection), m_secondConnection(secondConnection)
	{
		m_firstConnection->SetBelted(this);

		if (m_secondConnection != nullptr)
		{
			m_secondConnection->SetBelted(this);

			if (std::abs((m_firstConnection->GetPosition().X + m_firstConnection->GetScale().X/2.f) - (m_secondConnection->GetPosition().X + m_secondConnection->GetScale().X/2.f)) < 3.f)
			{
				Translate(Vec2{ -BELT_SEGMENT_WIDTH / 2.f, 0.f });
				m_scale = Vec2{ BELT_SEGMENT_WIDTH, (m_secondConnection->GetPosition().Y + m_secondConnection->GetScale().Y/2.f) - (m_firstConnection->GetPosition().Y + m_firstConnection->GetScale().Y/2.f)};
			}
			else {
				Translate(Vec2{ 0.f, -BELT_SEGMENT_WIDTH / 2.f });
				m_scale = Vec2{ BELT_SEGMENT_WIDTH, (m_secondConnection->GetPosition().X + m_secondConnection->GetScale().X/2.f) - (m_firstConnection->GetPosition().X + m_firstConnection->GetScale().X/2.f)};
				m_rotation = 90.f;
			}
		}
	}

	void Belt::Update(GLFWwindow* window)
	{
		if (!m_firstConnection) { Renderer::DestroyObject(this); return; }

		if (m_secondConnection)
		{
			m_beltSpeed = std::max(m_firstConnection->GetGearSpeed(), m_secondConnection->GetGearSpeed());
		}
		else {
			m_beltSpeed = m_firstConnection->GetGearSpeed();
		}
	}

	Vec2 Belt_BeltEndScale = Vec2{ 12.f };

	void Belt::Render(const Vec2& cameraPosition)
	{
		GameObject::Render(cameraPosition);

		//Render belt end graphics.
		Sprite sprite(Renderer::GetSpriteIdFromTextureId(BELT_END_SPRITE));
		GameObject tempOffset(Vec2{ 0.f }, Belt_BeltEndScale, nullptr);
		tempOffset.RotateAboutCenter(m_firstConnection->GetRotation());
		Vec2 posOff = tempOffset.GetRotationOffset();

		sprite.Render(cameraPosition, m_position + Vec2{ m_scale.X / 2.f, 0.f } - (Belt_BeltEndScale / 2.f) + posOff, Belt_BeltEndScale, m_firstConnection->GetRotation());
		sprite.Render(cameraPosition, m_position + Vec2{ m_scale.X / 2.f, m_scale.Y } - (Belt_BeltEndScale / 2.f) + posOff, Belt_BeltEndScale, m_firstConnection->GetRotation());
	}

	void Belt::RemoveConnection(Cog* cog)
	{
		if (m_firstConnection == cog)
		{
			m_firstConnection = nullptr;
		}

		if (m_secondConnection == cog)
		{
			m_secondConnection = nullptr;
		}
	}
}

