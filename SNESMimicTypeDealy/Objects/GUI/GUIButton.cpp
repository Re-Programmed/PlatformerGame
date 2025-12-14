#include "GUIButton.h"

#include "../../Rendering/DynamicSprite.h"
#include "../../Input/InputManager.h"
#include "../../Utils/CollisionDetection.h"
#include "../../Utils/Time/GameTime.h"
#include <algorithm>

#define BUTTON_FADE_SPEED_MULTIPLIER 5.f

#define BUTTON_FADE_CASCADE 0.25f

namespace GAME_NAME
{
	namespace Objects
	{
		namespace GUI
		{
			void GUIButton::Render(float zoom)
			{
				if (!m_active) { return; }

				Vec2 mousePos = InputManager::GetMouseScreenPosition();

				bool pwbBL = Utils::CollisionDetection::PointWithinBoxBL(mousePos, m_position, m_scale);

				Vec4 hoveredColors[4] = {
					m_baseColor,
					m_baseColor,
					m_baseColor,
					m_baseColor
				};


				//Hovered, render with color.
				if (m_lerpTimer > 0.f || pwbBL)
				{
					if (pwbBL)
					{
						if (m_lerpTimer < 1.f) { m_lerpTimer += (float)Utils::Time::GameTime::GetScaledDeltaTime() * BUTTON_FADE_SPEED_MULTIPLIER; }
					}
					else {
						m_lerpTimer -= (float)Utils::Time::GameTime::GetScaledDeltaTime() * BUTTON_FADE_SPEED_MULTIPLIER;
					}

					//Flip the darker part of the gradient based on where the mouse is.
					bool flipButtonGradient = (mousePos.X < m_position.X + m_scale.X / 2.f);

					//J maintains order to update verticies.
					int j = 0;
					//Change iteration direction (0 > 4) or (3 > 0) based on which side should be darker.
					for (int i = (flipButtonGradient ? 0 : 3); (flipButtonGradient ? (i < 4) : (i >= 0)); (flipButtonGradient ? (i++) : (i--)))
					{
						float cascadedLerpTimer = std::clamp(m_lerpTimer - BUTTON_FADE_CASCADE * i, 0.f, 1.f);

						//Scale hover timer by the lerp timer.
						Vec4 hoverColor = { std::lerp(m_baseColor.X, m_hoverColor.X, cascadedLerpTimer),
											std::lerp(m_baseColor.Y, m_hoverColor.Y, cascadedLerpTimer),
											std::lerp(m_baseColor.Z, m_hoverColor.Z, cascadedLerpTimer),
											m_hoverColor.W };
						hoveredColors[j++] = hoverColor;
					}
				}

				DynamicSprite* dSprite = new DynamicSprite(this->m_sprite->GetSpriteId());
				dSprite->UpdateTextureColor(hoveredColors);
				dSprite->Render(Vec2::Zero, m_position + m_scale, { -m_scale.X, -m_scale.Y }, 0.0F);
				delete dSprite;

			}
		}
	}
}