#include "LayerFlipObject.h"

#include "../TestGame.h"

namespace GAME_NAME::Objects
{
	void LayerFlipObject::Update(GLFWwindow* window)
	{
		if (!m_frontDisplayed)
		{
			if (TestGame::ThePlayer->GetPosition().Y > m_position.Y + m_scale.Y / 2.f + m_flipOffset)
			{
				m_frontDisplay->SetSprite(new Sprite(m_sprite->GetSpriteId()));
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_frontDisplay, true, 2, true));
				m_frontDisplayed = true;
			}
		}
		else {
			m_frontDisplay->SetScale(m_scale);
			m_frontDisplay->SetPosition(m_position);
			m_frontDisplay->SetTextureFlipped(m_textureFlipped);

			if (TestGame::ThePlayer->GetPosition().Y <= m_position.Y + m_scale.Y / 2.f + m_flipOffset)
			{
				Renderer::DestroyActiveObjectImmediate(m_frontDisplay); //Does not delete the object.
				m_frontDisplayed = false;
			}
		}
	}
}
