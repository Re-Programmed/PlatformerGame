#pragma once
#include "../../Utils/Math/Vec2.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

constexpr static float minimumCameraZoom = 0.3f; //Local to Camera
constexpr static float maximumCameraZoom = 2.f;	//Local to Camera

#define LIMIT_CAMERA_NEGATIVE true //Should the camera not be allowed to move into negative X and Y coords?

namespace GAME_NAME
{

	namespace Rendering
	{
		namespace Camera
		{
			using namespace MathUtils;
			//A camera stores a offset position and zoom to apply to all objects. (ex. A projection matrix.)
			class Camera
			{
			public:
				Camera(Vec2 position, float zoom);

				virtual void Update(GLFWwindow* window) {};

				Vec2 GetPosition();
				void SetPosition(Vec2 pos);
				/// <summary>
				/// Returns the global position of an object that should be rendered as a UI element relative to the camera. 
				/// (UI Positions are [-1, 1])
				/// </summary>
				/// <param name="pos"></param>
				/// <returns></returns>
				Vec2 UIToGlobal(Vec2 pos);
				Vec2 GlobalToUI(Vec2 pos);
			

				float GetZoom();
				void SetZoom(float zoom);

				/// <summary>
				/// Adds an amount to the current zoom.
				/// </summary>
				void Zoom(const float amount);

				void Translate(Vec2 translation);
		

			
				
			protected:
				Vec2 m_position;
				float m_zoom = 1.f;

			};
		}
	}
}