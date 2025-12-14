#include "InputManager.h"
#include "../Settings/AppDataFileManager.h"

namespace GAME_NAME
{

	int InputManager::m_keys[KEY_ARRAY_SIZE];
	GLFWwindow* InputManager::m_window;

	InputManager::KEY_STATE InputManager::m_keysDown[KEY_ARRAY_SIZE];

	std::vector<std::function<void(GLFWwindow*, double, double)>> InputManager::m_scrollCallbacks;

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
		return glfwGetKey(m_window, m_keys[key]);
	}

	const unsigned int InputManager::GetKey(int key)
	{
		return glfwGetKey(m_window, key);
	}

	const void GAME_NAME::InputManager::UpdateKeyStates()
	{
		for (int i = 0; i < KEY_ARRAY_SIZE; i++)
		{
			if (glfwGetKey(m_window, m_keys[i]))
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

	const GAME_NAME::InputManager::KEY_STATE InputManager::GetKeyUpDown(keyRef key)
	{
		return m_keysDown[key];
	}

	const float InputManager::GetJoystick()
	{

		/*for (int i = 0; i < 16; i++)
		{
			int present = glfwJoystickPresent(i);
			if (present == GLFW_TRUE)
			{
				std::cout << glfwGetJoystickName(i) << std::endl;
				continue;
			}

			break;
		}*/

		int count;
		const float* f = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
		//std::cout << glfwGetJoystickName(0) << ": " << count << " == " << f[0] << std::endl;

		for (int i = 0; i < count; i++)
		{
			//if (f[i] == GLFW_PRESS)
			//{
				//std::cout << glfwGetJoystickName(GLFW_JOYSTICK_1) << ": " << i << " == " << f[i] << std::endl;

			//}
		}

		return 0.0f;
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

		LoadKey(PLAYER_DROP_HELD_ITEM, GLFW_KEY_Q);

		LoadKey(PLAYER_OPEN_BACKPACK, GLFW_KEY_I);
		LoadKey(PLAYER_TOGGLE_FLASHLIGHT, GLFW_KEY_F);

		LoadKey(PLAYER_INTERACT, GLFW_KEY_E);
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
	}

	void InputManager::scrollCallback(GLFWwindow* window, double xScroll, double yScroll)
	{
		for (auto func : m_scrollCallbacks)
		{
			func(window, xScroll, yScroll);
		}
	}
}