#include "InputManager.h"
#include "../Settings/AppDataFileManager.h"

#include "../Utils/Time/GameTime.h"

namespace GAME_NAME
{


	int InputManager::m_keys[KEY_ARRAY_SIZE];
	GLFWwindow* InputManager::m_window;

	bool InputManager::m_usingMouse = true;
	MathUtils::Vec2 InputManager::m_lastMousePos = MathUtils::Vec2(0);

	InputManager::KEY_STATE InputManager::m_keysDown[KEY_ARRAY_SIZE];

	std::vector<std::function<void(GLFWwindow*, double, double)>> InputManager::m_scrollCallbacks;

	std::unordered_map<keyRef, std::vector<InputManager::ControllerInput>> InputManager::m_mappedControllerInputs = {
		{ keyRef::PLAYER_MOVE_RIGHT, { InputManager::ControllerInput(0, controllerRef::JOY_1_AXIS_X_POSITIVE), InputManager::ControllerInput(0, controllerRef::DPAD_RIGHT) } },
		{ keyRef::PLAYER_MOVE_LEFT,  { InputManager::ControllerInput(0, controllerRef::JOY_1_AXIS_X_NEGATIVE), InputManager::ControllerInput(0, controllerRef::DPAD_LEFT) } },
		{ keyRef::PLAYER_MOVE_UP,  { InputManager::ControllerInput(0, controllerRef::JOY_1_AXIS_Y_NEGATIVE) } },
		{ keyRef::PLAYER_MOVE_DOWN,  { InputManager::ControllerInput(0, controllerRef::JOY_1_AXIS_Y_POSITIVE) } },
		{ keyRef::PLAYER_FORCE_WALK,	 { InputManager::ControllerInput(0, controllerRef::JOY_1_PRESSED) } },
		{ keyRef::PLAYER_JUMP,		 { InputManager::ControllerInput(0, controllerRef::BUTTON_A) } },
		{ keyRef::PLAYER_INTERACT,	 { InputManager::ControllerInput(0, controllerRef::BUTTON_B) } },
		{ keyRef::PLAYER_ATTACK,	 { InputManager::ControllerInput(0, controllerRef::BUTTON_X) } },
		{ keyRef::PLAYER_USE_ITEM,	 { InputManager::ControllerInput(0, controllerRef::BUTTON_R), InputManager::ControllerInput(0, controllerRef::BUTTON_ZR) } },
		{ keyRef::PLAYER_OPEN_BACKPACK,	 { InputManager::ControllerInput(0, controllerRef::BUTTON_PLUS) } },
		{ keyRef::DEFAULT_PAUSE_GAME,	 { InputManager::ControllerInput(0, controllerRef::BUTTON_HOME) } },
		{ keyRef::PLAYER_SCREEN_INVENTORY_SCROLL_THROUGH,	 { InputManager::ControllerInput(0, controllerRef::BUTTON_L), InputManager::ControllerInput(0, controllerRef::BUTTON_ZL) } },
		{ keyRef::PLAYER_DROP_HELD_ITEM,	 { InputManager::ControllerInput(0, controllerRef::BUTTON_Y) } },

	};

	void InputManager::Init(GLFWwindow* window)
	{
		//LOAD KEY INPUT SETTINGS FROM SAVE

		/*if no keys are saved*/loadDefaultKeys();

		m_window = window;

		glfwSetScrollCallback(m_window, InputManager::scrollCallback);
	}

	/*void InputManager::SetScrollCallback(Window* window, void (*f)(Window*, double, double))
	{
		m_scrollCallback = f;
	}

	void InputManager::ScrollCallback(Window* window, double offsetx, double offsety)
	{
		m_scrollCallback(window, offsetx, offsety);
	}*/

	const unsigned int InputManager::GetKey(keyRef key)
	{
		//----------------------------
		//Is a controller being used, is they key mapped to a controller, and is the joystick/button pressed pass the dead zone for this key?
		//----------------------------
		if (m_mappedControllerInputs.contains(key))
		{
			for (ControllerInput& controllerInput : m_mappedControllerInputs[key])
			{
				if (!ControllerAvailable(controllerInput.ControllerId)) { continue; }
				//Checking the first joystick.
				if (controllerInput.Ref == controllerRef::JOY_1_AXIS_X_NEGATIVE || controllerInput.Ref == controllerRef::JOY_1_AXIS_X_POSITIVE || controllerInput.Ref == controllerRef::JOY_1_AXIS_Y_NEGATIVE || controllerInput.Ref == controllerRef::JOY_1_AXIS_Y_POSITIVE)
				{
					MathUtils::Vec2 joyPos = GetJoystick(controllerInput.ControllerId, 0);

					bool usedJoy = false;

					switch (controllerInput.Ref)
					{
					case JOY_1_AXIS_X_NEGATIVE:
						if (joyPos.X < -JOYSTICK_DEADZONE) { usedJoy = true; }
						break;
					case JOY_1_AXIS_X_POSITIVE:
						if (joyPos.X > JOYSTICK_DEADZONE) { usedJoy = true; }
						break;
					case JOY_1_AXIS_Y_NEGATIVE:
						if (joyPos.Y < -JOYSTICK_DEADZONE) { usedJoy = true; }
						break;
					case JOY_1_AXIS_Y_POSITIVE:
						if (joyPos.Y > JOYSTICK_DEADZONE) { usedJoy = true; }
						break;
					default:
						break;
					}

					if (usedJoy)
					{
						//Hide the cursor if you start moving.

						glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
						if (m_usingMouse)
						{
							m_usingMouse = false;
							m_lastMousePos = GetMouseScreenPosition();
						}

						return true;
					}
				}
				//Checking the second joystick.
				else if (controllerInput.Ref == controllerRef::JOY_2_AXIS_X_NEGATIVE || controllerInput.Ref == controllerRef::JOY_2_AXIS_X_POSITIVE || controllerInput.Ref == controllerRef::JOY_2_AXIS_Y_NEGATIVE || controllerInput.Ref == controllerRef::JOY_2_AXIS_Y_POSITIVE)
				{
					MathUtils::Vec2 joyPos = GetJoystick(controllerInput.ControllerId, 1);

					bool usedJoy = false;

					switch (controllerInput.Ref)
					{
					case JOY_2_AXIS_X_NEGATIVE:
						if (joyPos.X < -JOYSTICK_DEADZONE) { usedJoy = true; }
						break;
					case JOY_2_AXIS_X_POSITIVE:
						if (joyPos.X > JOYSTICK_DEADZONE) { usedJoy = true; }
						break;
					case JOY_2_AXIS_Y_NEGATIVE:
						if (joyPos.Y < -JOYSTICK_DEADZONE) { usedJoy = true; }
						break;
					case JOY_2_AXIS_Y_POSITIVE:
						if (joyPos.Y > JOYSTICK_DEADZONE) { usedJoy = true; }
						break;
					default:
						break;
					}

					//JOY 2 ACTS AS THE MOUSE, NO REASON TO CHECK USING MOUSE.
				}
				//Should check a controller button.
				else {
					if (GetControllerButton(controllerInput.ControllerId, controllerInput.Ref)) { return true; }
				}
			}
		}

		//0 and 1 designate mouse button inputs.
		if (m_keys[key] < 2)
		{
			return GetMouseButton(m_keys[key]);
		}

		//If no controller is in use or no controller is being pressed check the keyboard.
		return glfwGetKey(m_window, m_keys[key]);
	}

	const unsigned int InputManager::GetKey(int key)
	{
		if (key < 2)
		{
			return glfwGetMouseButton(m_window, key);
		}

		return glfwGetKey(m_window, key);
	}

#define CONTROLLER_CURSOR_MOVEMENT_SPEED 700.0

	const void GAME_NAME::InputManager::UpdateKeyStates()
	{
		if (!m_usingMouse)
		{
			//Allow scrolling via the joystick.
			MathUtils::Vec2 joy1 = GetJoystick(0, 0);
			if (joy1.Y > JOYSTICK_DEADZONE)
			{
				scrollCallback(m_window, 0.0, joy1.Y);
			}
			else if (joy1.Y < -JOYSTICK_DEADZONE)
			{
				scrollCallback(m_window, 0.0, joy1.Y);
			}

			//Allow moving the mouse with the joystick.
			MathUtils::Vec2 joy2 = GetJoystick(0, 1);
			if (joy2.X < -JOYSTICK_DEADZONE || joy2.X > JOYSTICK_DEADZONE || joy2.Y < -JOYSTICK_DEADZONE || joy2.Y > JOYSTICK_DEADZONE)
			{
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				double xPos = 0.0; double yPos = 0.0; glfwGetCursorPos(m_window, &xPos, &yPos);

				if (joy2.X > JOYSTICK_DEADZONE || joy2.X < -JOYSTICK_DEADZONE)
				{
					xPos += CONTROLLER_CURSOR_MOVEMENT_SPEED * Utils::Time::GameTime::DeltaTime::GetDeltaTime() * joy2.X;
				}

				if (joy2.Y > JOYSTICK_DEADZONE || joy2.Y < -JOYSTICK_DEADZONE)
				{
					yPos += CONTROLLER_CURSOR_MOVEMENT_SPEED * Utils::Time::GameTime::DeltaTime::GetDeltaTime() * joy2.Y;
				}

				glfwSetCursorPos(m_window, xPos, yPos);
				m_lastMousePos = GetMouseScreenPosition();
			}
			else {
				

				MathUtils::Vec2 mousePos = GetMouseScreenPosition();
				if (m_lastMousePos.X != mousePos.X || m_lastMousePos.Y != mousePos.Y)
				{
					glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					m_usingMouse = true;
				}
			}
		}

		for (int i = 0; i < KEY_ARRAY_SIZE; i++)
		{
			if (GetKey(static_cast<keyRef>(i)))
			{
				if (m_keysDown[i] & (KEY_STATE_NONE | KEY_STATE_RELEASED))
				{
					m_keysDown[i] = KEY_STATE_PRESSED;
					continue;
				}

				m_keysDown[i] = KEY_STATE_HELD;
				continue;
			}

			if (m_keysDown[i] & (KEY_STATE_HELD | KEY_STATE_PRESSED))
			{
				m_keysDown[i] = KEY_STATE_RELEASED;
				continue;
			}

			m_keysDown[i] = KEY_STATE_NONE;
		}
	}

	const std::string InputManager::GetControllerName(int controllerIndex)
	{
		int present = glfwJoystickPresent(controllerIndex);
		if (present == GLFW_TRUE)
		{
			return std::string(glfwGetJoystickName(controllerIndex));
		}

		return "";
	}

	const MathUtils::Vec2 InputManager::GetJoystick(int controllerIndex, int joystickIndex)
	{
		int count;
		const float* f = glfwGetJoystickAxes(controllerIndex, &count);
		//std::cout << glfwGetJoystickName(0) << ": " << count << " == " << f[0] << std::endl;

		if (joystickIndex < count)
		{
			return MathUtils::Vec2(f[joystickIndex * 2], f[joystickIndex * 2 + 1]);
		}

		return 0.0f;
	}

	const bool InputManager::GetControllerButton(int controllerIndex, controllerRef button)
	{
		int count;
		const unsigned char* buttons = glfwGetJoystickButtons(controllerIndex, &count);

		//Offset because of the values of the joystick inputs in the enum.
		unsigned int buttonIndex = button - controllerRef::BUTTON_B;
		if (buttonIndex < count)
		{
			return buttons[buttonIndex] == GLFW_PRESS;
		}

		return false;
	}

	const GAME_NAME::InputManager::KEY_STATE InputManager::GetKeyUpDown(keyRef key)
	{
		return m_keysDown[key];
	}

	MathUtils::Vec2 InputManager::GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(m_window, &x, &y);
		return MathUtils::Vec2(static_cast<float>(x), static_cast<float>(y));
		
	}

	MathUtils::Vec2 InputManager::GetMouseScreenPosition()
	{
		MathUtils::Vec2 screenCoords = GetMousePosition();
		int wWidth, wHeight;
		glfwGetWindowSize(m_window, &wWidth, &wHeight);
		MathUtils::Vec2 normalizedScreenCoords = MathUtils::Vec2(screenCoords.X / wWidth, 1 - (screenCoords.Y) / wHeight);

		return MathUtils::Vec2(TargetResolutionX * normalizedScreenCoords.X, TargetResolutionY * normalizedScreenCoords.Y);
	}

	void InputManager::SetMouseScreenPosition(MathUtils::Vec2 mousePosition)
	{
		int wWidth, wHeight;
		glfwGetWindowSize(m_window, &wWidth, &wHeight);
		
		MathUtils::Vec2 screenCoords((mousePosition.X / TargetResolutionX) * wWidth, ((TargetResolutionY - mousePosition.Y)/TargetResolutionY) * wHeight);

		glfwSetCursorPos(m_window, (double)(screenCoords.X), (double)(screenCoords.Y));
	}

	void InputManager::SaveAllKeybinds()
	{
		for (int i = 0; i < KEY_ARRAY_SIZE; i++)
		{
			AppData::AppDataFileManager::SetValue("settings\\keybinds.dat", KeybindNames[i].data(), std::to_string(m_keys[i]));
		}
	}


	MathUtils::Vec2 InputManager::GetMouseWorldPosition(Rendering::Camera::Camera* camera)
	{
		return GetMouseScreenPosition() + camera->GetPosition();
	}

#define LoadKey(bind, default_k) m_keys[bind] = std::stoi(AppData::AppDataFileManager::GetValue("settings\\keybinds.dat", KeybindNames[bind].data(), std::to_string(default_k)));


#define CONTROLLER_INPUT_PREFIX "CONTROLLER_"

	void InputManager::loadDefaultKeys()
	{
		LoadKey(PLAYER_MOVE_UP, GLFW_KEY_W);
		LoadKey(PLAYER_MOVE_DOWN, GLFW_KEY_S);
		LoadKey(PLAYER_MOVE_LEFT, GLFW_KEY_A);
		LoadKey(PLAYER_MOVE_RIGHT, GLFW_KEY_D);

		LoadKey(PLAYER_JUMP, GLFW_KEY_SPACE);

		LoadKey(PLAYER_FORCE_WALK, GLFW_KEY_LEFT_SHIFT);

		LoadKey(PLAYER_SCREEN_INVENTORY_SLOT_1, GLFW_KEY_1);
		LoadKey(PLAYER_SCREEN_INVENTORY_SLOT_2, GLFW_KEY_2);
		LoadKey(PLAYER_SCREEN_INVENTORY_SLOT_3, GLFW_KEY_3);
		LoadKey(PLAYER_SCREEN_INVENTORY_SCROLL_THROUGH, GLFW_KEY_TAB);

		LoadKey(PLAYER_DROP_HELD_ITEM, GLFW_KEY_Q);

		LoadKey(PLAYER_OPEN_BACKPACK, GLFW_KEY_I);
		LoadKey(PLAYER_TOGGLE_FLASHLIGHT, GLFW_KEY_F);

		LoadKey(PLAYER_INTERACT, GLFW_KEY_E);

		LoadKey(PLAYER_USE_ITEM, GLFW_MOUSE_BUTTON_1);
		LoadKey(PLAYER_ATTACK, GLFW_MOUSE_BUTTON_2);
#if _DEBUG
		LoadKey(PLAYER_DEBUG, GLFW_KEY_P);

		LoadKey(DEBUG_EDITOR_SELECT, GLFW_KEY_V);

		LoadKey(DEBUG_OBJECT_MENU, GLFW_KEY_TAB);

		LoadKey(PLAYER_DEBUG_ADD_SPEED, GLFW_KEY_LEFT_SHIFT);

		LoadKey(DEBUG_SET_OBJECT_X, GLFW_KEY_X);
		LoadKey(DEBUG_SET_OBJECT_Y, GLFW_KEY_Y);

		LoadKey(DEBUG_REFRESH_LEVEL_FILES, GLFW_KEY_M);
		LoadKey(PLAYER_DEBUG_TOGGLE_FLIGHT, GLFW_KEY_V);
#endif

		//Default Keys (Cannot be changed in settings)
		m_keys[DEFAULT_PAUSE_GAME] = GLFW_KEY_ESCAPE;

		//Load controller inputs.
		for (auto& [ref, controller] : m_mappedControllerInputs)
		{
			unsigned int i = 0;
			for (ControllerInput& controllerInput : controller)
			{
				std::string data = AppData::AppDataFileManager::GetValue("settings\\keybinds.dat", std::string(CONTROLLER_INPUT_PREFIX).append(KeybindNames[ref].data()).append("_").append(std::to_string(i)), std::to_string(controllerInput.ControllerId).append(":").append(std::to_string(controllerInput.Ref)));

				//data will be "controllerId:keyRef"
				controllerInput.ControllerId = std::stoi(data.substr(0, data.find_first_of(":")));
				controllerInput.Ref = static_cast<controllerRef>(std::stoi(data.substr(data.find_first_of(":") + 1)));
				i++;
			}
		}

	}

	void InputManager::scrollCallback(GLFWwindow* window, double xScroll, double yScroll)
	{
		for (auto func : m_scrollCallbacks)
		{
			func(window, xScroll, yScroll);
		}
	}
}