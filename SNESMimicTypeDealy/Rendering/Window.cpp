#include "Window.h"
#include <cstdlib>
#include <iostream>
#include "./BufferManager.h"
#include "./Renderers/Renderer.h"
#include "./Sprite.h"
#include "../Objects/GameObject.h"
#include "../Resources/AssetManager.h"
#include "../Input/InputManager.h"
#include "../Utils/UpdateManager.h"
#if _DEBUG
#include "../Debug/DebugLog.h"
#endif
#include "../Audio/SoundManager.h"
#include "../Utils/Time/GameTime.h"
#include "../Objects/GUI/GUIManager.h"
#include "../Settings/SettingsGlobals.h"

//The expected amount of time that should pass between each frame.
#define _G_EXPECTED_FRAME_OBJ_UPDATE 0.0088f

#define _WS_WINDOW_TITLE "Platformer"	//Window title.
#define _WS_WINDOW_VSYNC 1				//Sync window to frame rate of monitor or OS. [ADD A SETTING TO CHANGE THIS IN GAME FOR USERS.]

namespace GAME_NAME
{
	Vec2* lastWindowSize;

	/// <summary>
	/// Set to the first window created.
	/// </summary>
	Window* Window::INSTANCE = nullptr;

	void windowSizeCallback(GLFWwindow* window, int width, int height)
	{
		//Window was minimized or has no size.
		if (width == 0 || height == 0) {
			return;
		}

		if (width < 355)
		{
			width = 355;
		}

		//Figure out what the width and height should be based off of each other.
		float adjustedWindowW = ((float)height * (FORCE_WINDOW_RATIO_X / FORCE_WINDOW_RATIO_Y));
		float adjustedWindowH = ((float)width * (FORCE_WINDOW_RATIO_Y / FORCE_WINDOW_RATIO_X));

		//Update the window size that was not changed to fit the ratio.
		if (lastWindowSize != nullptr && lastWindowSize->X != width)
		{
			glfwSetWindowSize(window, width, (int)adjustedWindowH);
		}
		else {
			glfwSetWindowSize(window, (int)adjustedWindowW, height);
		}

		//Update the last window size to the new size.
		lastWindowSize = new Vec2(width, height);

		//Set sprite scaling. (Maintains the size of sprites and camera zoom when the window is changed.)
		Sprite::SetResolution(Vec2(width, height));

		AppData::Settings::SettingsGlobals::WindowResolutionX.Value = (uint16_t)adjustedWindowW;
		AppData::Settings::SettingsGlobals::WindowResolutionY.Value = (uint16_t)adjustedWindowH;
	}

	GLFWwindow* Window::GetWindow()
	{
		return m_glWindow;
	}

	

	Window::Window(bool fullscreen, GAME_NAME::Game::Game* game) : m_game(game), m_fullscreen(fullscreen)
	{
		/*----------------------
			ADD INITS
		------------------------*/

		if (INSTANCE == nullptr)
		{
			INSTANCE = this;
		}

		int count;
		GLFWmonitor* primaryMonitor = glfwGetMonitors(&count)[0]; //Change later to choose a monitor. [FIX]
		
		int width, height;
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
		width = mode->width;
		height = mode->height;


		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); //DISABLE RESIZING

		std::string title = _WS_WINDOW_TITLE;
		this->m_glWindow = glfwCreateWindow(width, height, title.c_str(), fullscreen ? primaryMonitor : NULL, NULL);
		#if _DEBUG
		DEBUG::DebugLog::Log("Created Window: " + title + "\n", true, ";1;4");
		#endif

		if (!this->m_glWindow)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(this->m_glWindow);
		//Vysnc enabled?
		glfwSwapInterval(_WS_WINDOW_VSYNC);

		gladLoadGL();

		glfwSetWindowSizeCallback(m_glWindow, windowSizeCallback);

		//glViewport(0, 0, width, height);

		glEnable(GL_TEXTURE_2D);

		//BLENDING
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glOrtho(0, width, 0, height, 0, 100);
		glMatrixMode(GL_MODELVIEW);

		if (AppData::Settings::SettingsGlobals::WindowResolutionX.Value > 0)
		{
			width = AppData::Settings::SettingsGlobals::WindowResolutionX.Value;
		}

		if (AppData::Settings::SettingsGlobals::WindowResolutionY.Value > 0)
		{
			height = AppData::Settings::SettingsGlobals::WindowResolutionY.Value;
		}

		std::cout << "[[ GOT WINDOW SCALE TO BE: " << width << ", " << height << " ]]\n";

		windowSizeCallback(m_glWindow, width, height);
		glfwSetWindowSizeLimits(m_glWindow, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);

		BufferManager::GenBuffers();

		InputManager::Init(m_glWindow);

		Audio::SoundManager::Init();

		m_game->Init(m_glWindow); //INITILIZE GAME LAST

		//TEST?

		///CHUNK DATA HAS BACKGROUND INFO (THIS NEEDS CHANGED BECAUSE THE CHUNKS OBJECTS NEED PARALLAX!)
		
		///WHAT ABOUT MOVING OBJECTS THAT EXCEED CHUNKS LIKE ENEMIES AND THE PLAYER!!!!! (WE CANT KEEP UPDATING WHAT CHUNK THEY ARE IN, WE NEED TO SPECIFICALLY CHECK THEIR POSITION)

		//Should we be fullscreen?
		SetFullscreen(AppData::Settings::SettingsGlobals::Fullscreen.Value);
	}

	Window::~Window()
	{
		Audio::SoundManager::DeInit();

		glfwDestroyWindow(this->m_glWindow);
	}

	//float deltaUpdate = 0.f;

	void Window::Render()
	{
		Time::GameTime::Update();

		bool windowIsActive = glfwGetWindowAttrib(m_glWindow, GLFW_FOCUSED)
			&& !glfwGetWindowAttrib(m_glWindow, GLFW_ICONIFIED)
			&& glfwGetWindowAttrib(m_glWindow, GLFW_VISIBLE);
		if (!windowIsActive)
		{
			glfwPollEvents();
			return;
		}

		//deltaUpdate += GAME_NAME::Utils::Time::GameTime::GetScaledDeltaTime();

		//REMOVED
		//Check if the amount of time elapsed is over the expected amount of time per frame.
		//If it is, update all objects, otherwise just render them.
		//if (deltaUpdate >= _G_EXPECTED_FRAME_OBJ_UPDATE) { deltaUpdate = 0.f; Renderer::UpdateObjects = true; }
		//else { Renderer::UpdateObjects = false; }

		update();

		glClear(GL_COLOR_BUFFER_BIT);

		//Draw Color
		//glColor3f(1, 0, 0);

		if (m_game->RenderBG)
		{
			Renderer::Render(m_game->GetCamera(), lastWindowSize, RENDER_LAYER_BG, m_glWindow, 2.f);
		}

		Renderer::Render(m_game->GetCamera(), lastWindowSize, RENDER_LAYER_OBJECTS, m_glWindow);
		Renderer::Render(m_game->GetCamera(), lastWindowSize, RENDER_LAYER_ACTIVE_OBJECTS, m_glWindow);

		if (m_game->RenderFront)
		{
			Renderer::Render(m_game->GetCamera(), lastWindowSize, RENDER_LAYER_OBJECTS_FRONT, m_glWindow);
		}

		if (GUI::GUIManager::MenuIsOpen)
		{
			Renderer::Render(m_game->GetCamera(), lastWindowSize, RENDER_LAYER_GUI, m_glWindow);
			
			if (InputManager::GetMouseButton(0) || InputManager::GetMouseButton(1) || InputManager::GetControllerButton(0, controllerRef::BUTTON_ZL) || InputManager::GetControllerButton(0, controllerRef::BUTTON_L) || InputManager::GetControllerButton(0, controllerRef::BUTTON_ZR) || InputManager::GetControllerButton(0, controllerRef::BUTTON_R))
			{
				GUI::GUIManager::ButtonClickEvent(InputManager::GetMouseScreenPosition(), InputManager::GetMouseButton(1));
			}
		}

		glfwSwapBuffers(this->m_glWindow);
		glfwPollEvents();

		lateUpdate();
	}

	void Window::SetFullscreen(bool fullscreen)
	{
		if (fullscreen == this->m_fullscreen) { return; }

		this->m_fullscreen = fullscreen;

		int count;
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetMonitors(&count)[0] /*Change later to choose a monitor.*/);
		int mWidth, mHeight, mPosX, mPosY;
		int wWidth = mode->width - INITIAL_WINDOW_FULLSCREEN_OFF_PADDING_X;
		int wHeight = mode->height - INITIAL_WINDOW_FULLSCREEN_OFF_PADDING_Y;
		glfwGetMonitorWorkarea(glfwGetMonitors(&count)[0] /*Change later to choose a monitor.*/, &mPosX, &mPosY, &mWidth, &mHeight);
		glfwSetWindowMonitor(m_glWindow, fullscreen ? glfwGetMonitors(&count)[0] : NULL, mWidth/2 - wWidth/2 + mPosX, mHeight/2 - wHeight/2 + mPosY + INITIAL_WINDOW_FULLSCREEN_OFF_OFFSET_Y, wWidth, wHeight, mode->refreshRate);

		windowSizeCallback(m_glWindow, mode->width, mode->height);

		glfwSetWindowAttrib(m_glWindow, GLFW_RESIZABLE, m_fullscreen ? GLFW_FALSE : GLFW_TRUE);

		AppData::Settings::SettingsGlobals::Fullscreen.Value = m_fullscreen;
	}

	void Window::SetClearColor(Color color)
	{
		glClearColor(color.X, color.Y, color.Z, color.W);
	}

	bool F11press = false;
	void Window::update()
	{
		if (glfwGetKey(m_glWindow, GLFW_KEY_F11))
		{
			if (!F11press)
			{
				std::cout << "UPDATING" << std::endl;
				F11press = true;

				SetFullscreen(!this->m_fullscreen);
			}
		}
		else {
			F11press = false;
		}

		GAME_NAME::Utils::UpdateManager::Update(m_glWindow);
	}

	void Window::lateUpdate()
	{
		m_game->LateUpdate(m_glWindow);
	}
}