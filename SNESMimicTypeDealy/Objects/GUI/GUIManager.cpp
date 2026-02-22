#include "GUIManager.h"
#include "../../Utils/CollisionDetection.h"

#include "../../Input/InputManager.h"

#include "../../!TestGame/TestGame.h"

namespace GAME_NAME
{
	namespace Objects
	{
		namespace GUI
		{
			std::unordered_map<uint32_t, GUIButton*> GUIManager::m_buttons;
			int32_t GUIManager::m_controllerSelectedButton = -1;
			bool GUIManager::m_joyHeld = false;

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
				for (auto[buttonID, gb] : m_buttons)
				{
					if (!gb->GetActive()) { continue; }

					if (!wasRightClick && gb->OnClick == nullptr) { return; }
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

			void GUIManager::UpdateControllerInput()
			{
				//Using controller.
				if (!InputManager::GetUsingMouse() && MenuIsOpen)
				{
					Vec2 joy1 = InputManager::GetJoystick(0, 0);
					if (joy1.X > JOYSTICK_DEADZONE || joy1.X < -JOYSTICK_DEADZONE)
					{
						//If the player isn't in a menu, set mouse to the middle.
						if (!TestGame::ThePlayer->GetFrozen()) { InputManager::SetMouseScreenPosition(Vec2{ TargetResolutionX / 2.f, TargetResolutionY / 2.f }); glfwSetInputMode(TestGame::INSTANCE->FirstWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); return; }

						if (!m_joyHeld)
						{
							m_controllerSelectedButton += (joy1.X > 0.f ? 1.f : -1.f);
							if (m_controllerSelectedButton >= m_buttons.size())
							{
								m_controllerSelectedButton = 0;
							}
							else if (m_controllerSelectedButton < 0)
							{
								m_controllerSelectedButton = m_buttons.size() - 1;
							}

							int i = 0;
							for (auto& [buttonID, button] : m_buttons)
							{
								if (i == m_controllerSelectedButton)
								{
									if (button != nullptr)
									{
										//Move the mouse to be selecting the next button.
										InputManager::SetMouseScreenPosition(button->GetPosition() + button->GetScale() / 2.f);

									}
								}

								i++;
							}

							m_joyHeld = true;
						}

						return;
					}
				}
				else {
					m_controllerSelectedButton = -1;
				}

				m_joyHeld = false;
			}
		}
	}
}