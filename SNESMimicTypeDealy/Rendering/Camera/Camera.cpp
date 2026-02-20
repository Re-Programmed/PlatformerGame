#include "Camera.h"
#include <algorithm>
#include <iostream>
#include "../../glad/include/glad/glad.h"

namespace GAME_NAME
{

	namespace Rendering
	{
		namespace Camera
		{

			Camera::Camera(Vec2 position, float zoom)
				: m_position(position), m_zoom(zoom)
			{
				
			}

			Vec2 Camera::GetPosition()
			{
				return m_position;
			}

			void Camera::SetPosition(Vec2 pos)
			{
#if LIMIT_CAMERA_NEGATIVE
				if (pos.X >= 0) { m_position.X = pos.X; }
				if (pos.Y >= 0) { m_position.Y = pos.Y; }
#else
				m_position = pos;
#endif
			}

		
			Vec2 Camera::UIToGlobal(Vec2 pos)
			{
				return m_position + (pos * m_zoom);
			}

			Vec2 Camera::GlobalToUI(Vec2 pos)
			{
				return (pos - m_position) / m_zoom;
			}

			float Camera::GetZoom()
			{
				return m_zoom;
			}

			void Camera::SetZoom(float zoom)
			{
				float pre_zoom = m_zoom;
				float post_zoom_change = std::clamp(zoom, minimumCameraZoom, maximumCameraZoom);
				m_zoom = post_zoom_change;

				//Update Projection Matrix
				glMatrixMode(GL_PROJECTION);
				glScalef(1 - (pre_zoom - post_zoom_change), 1 - (pre_zoom - post_zoom_change), 1);
				glMatrixMode(GL_MODELVIEW);
			}

			void Camera::Zoom(const float amount)
			{
				SetZoom(m_zoom + amount);

			}


		


			void Camera::Translate(Vec2 translation)
			{
				SetPosition(m_position + translation);
			}

		}
	}
}