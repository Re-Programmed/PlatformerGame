#include "GUIManager.h"
#include "../../Utils/CollisionDetection.h"
#pragma once

namespace GAME_NAME
{
	namespace Objects
	{
		namespace GUI
		{
			std::vector<GUIButton*> GUIManager::m_buttons;

			GUIManager::GUI_ELEMENT_TYPE GUIManager::GetGUIComponentFromCode(std::string code)
			{
				for (int i = 0; i < GUIMANAGER_M_ELEMENT_CODES_LENGTH; i++)
				{
					if (m_elementCodes[i] == code)
					{
						return (GUIManager::GUI_ELEMENT_TYPE)i;
					}
				}

				return GUIManager::EL_BLANK;
			}

			void GUIManager::ButtonClickEvent(Vec2 position, bool wasRightClick)
			{
				for (GUIButton* gb : m_buttons)
				{
					if (!gb->GetActive()) { continue; }

					if (wasRightClick && gb->OnRightClick == nullptr) { continue; }
					if (Utils::CollisionDetection::PointWithinBoxBL(position, gb->GetPosition(), gb->GetScale()))
					{
						if(wasRightClick)
						{
							(*gb->OnRightClick)(gb->GetButtonId());
						}
						else {
							(*gb->OnClick)(gb->GetButtonId());
						}
						return;
					}
				}
			}
		}
	}
}