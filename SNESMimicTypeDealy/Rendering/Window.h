#pragma once
#include "../Utils/Math/Vec2.h"
#include "../Utils/Math/Vec4.h"
#include "../glad/include/glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Camera/Camera.h"
#include "Camera/MoveableCamera.h"
#include "../Game/Game.h"

namespace GAME_NAME
{
#define INITIAL_WINDOW_FULLSCREEN_OFF_PADDING_X 50
#define INITIAL_WINDOW_FULLSCREEN_OFF_PADDING_Y 85
#define INITIAL_WINDOW_FULLSCREEN_OFF_OFFSET_Y 10

#define FORCE_WINDOW_RATIO_X 16.f
#define FORCE_WINDOW_RATIO_Y 9.f

	using namespace Rendering;
	using namespace MathUtils;

	typedef Vec4 Color;

	class Window	//The game window.
	{
	public:
		static Window* INSTANCE;

		GLFWwindow* GetWindow();

		Window(bool fullscreen, GAME_NAME::Game::Game* game);
		~Window();

		void Render();
		/// <summary>
		/// Returns if the window should close.
		/// </summary>
		inline bool ShouldClose() const
		{
			return glfwWindowShouldClose(this->m_glWindow);
		}

		/// <summary>
		/// Sets if the window should be displayed in fullscreen or windowed mode.
		/// </summary>
		void SetFullscreen(bool fullscreen);

		inline bool GetFullscreen()
		{
			return m_fullscreen;
		}

		/// <summary>
		/// Sets the clear color for the window.
		/// </summary>
		void SetClearColor(Color color);


	private:


		GAME_NAME::Game::Game* const m_game;	//The current game.

		GLFWwindow* m_glWindow;					//The current GLFW window.

		bool m_fullscreen;						//Is the window fullscreen?

		void update();
		void lateUpdate();
	};

}