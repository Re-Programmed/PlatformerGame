#include "LevelInfoDisplay.h"
#include "../TestGame.h"
#include "../../Objects/GUI/Text/TextRenderer.h"

namespace GAME_NAME::Level
{
	std::unique_ptr<StaticGUIElement> LevelInfoDisplay::m_pointsObjects[6] { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	void LevelInfoDisplay::UpdateDSP(uint16_t points)
	{
		/*for (int i = 0; i < 6; i++)
		{
			if (m_pointsObjects[i] != nullptr)
			{
				Renderer::UnloadGUIElement(m_pointsObjects[0].get());
			}
		}

		Vec2 dPos = Vec2(10, 167.5f);
		Text::TextRenderer::RenderedDigit rd = Text::TextRenderer::RenderNumber(points, dPos, 0.35f, -11.5f, 6);
		*/

	}
}
