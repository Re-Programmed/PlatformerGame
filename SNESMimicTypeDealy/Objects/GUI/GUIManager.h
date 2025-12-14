#pragma once

#ifndef _GUI_MANAGER
#define _GUI_MANAGER
#include "../../Utils/Math/Vec2.h"
#include <vector>
#include "GUIButton.h"


namespace GAME_NAME
{
	namespace Objects
	{
		namespace GUI
		{
			using namespace MathUtils;
			/// <summary>
			/// Used for determining GUI events such as button presses.
			/// </summary>
			class GUIManager
			{
			public:
#define GUIMANAGER_M_ELEMENT_CODES_LENGTH 6
				/// <summary>
				/// Types of GUI elements that can be created.
				/// </summary>
				enum GUI_ELEMENT_TYPE
				{
					EL_TEXT,		//Text object.
					EL_BUTTON,		//Clickable button.
					EL_NUM,			//Number display.
					EL_BLANK,		//Object with texture.
					EL_PROGRESS,	//Progress Bar.
					EL_SCROLLAREA	//Scroll Area.
				};
				
			private:
				static inline const std::string m_elementCodes[GUIMANAGER_M_ELEMENT_CODES_LENGTH] {
					"txt",
					"btn",
					"num",
					"obj",
					"prg",
					"scr"
				};

			public:
				static inline bool PreventMenus = false;
				static inline bool MenuIsOpen = false;

				/// <summary>
				/// Returns the GUI element type based on the given code.
				/// </summary>
				static GUI_ELEMENT_TYPE GetGUIComponentFromCode(std::string code);

				/// <summary>
				/// Call this everytime a click occurs. Updates all buttons accordingly.
				/// </summary>
				static void ButtonClickEvent(Vec2 position, bool wasRightClick);

				/// <summary>
				/// Register a button so it can get clicked.
				/// Setting configureButtonId to false will cause the button to retain whatever buttonId it was initilized with. When this variable 
				/// is true, the button id is set according to how many buttons exist, ensuring that configured buttons have unique ids.
				/// </summary>
				/// <param name="btn"></param>
				/// 
				/// <returns></returns>
				static inline unsigned int RegisterButton(GUIButton* btn, bool configureButtonId = true)
				{
					m_buttons.push_back(btn);
					if (configureButtonId) { btn->SetButtonId(static_cast<unsigned int>(m_buttons.size()) - 1); }
					return static_cast<unsigned int>(m_buttons.size()) - 1;
				}

				/// <summary>
				/// Always unregister buttons before deleting them.
				/// </summary>
				/// <param name="index"></param>
				static inline void UnregisterButton(unsigned int index)
				{
					m_buttons.erase(m_buttons.begin() + index);
				}

				static inline void UnregisterButton(GUIButton* btn)
				{
					for (int i = 0; i < m_buttons.size(); i++)
					{
						if (m_buttons[i] == btn)
						{
							m_buttons.erase(m_buttons.begin() + i);
							return;
						}
					}
				}

			private:
				/// <summary>
				/// All buttons on screen.
				/// </summary>
				static std::vector<GUIButton*> m_buttons;
			};
		}
	}
}
#endif