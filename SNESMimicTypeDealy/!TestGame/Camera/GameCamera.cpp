#include "GameCamera.h"
#include "../../Utils/Time/GameTime.h"
#include "../../Rendering/Sprite.h"

#if _DEBUG
#include "../../Input/InputManager.h"
#endif

namespace GAME_NAME
{
	namespace Camera
	{
		GameCamera::GameCamera()
			: Rendering::Camera::Camera(Vec2::Zero, 1.f), m_targetZoom(-1), m_offset({ 0, 0 }), m_followPlayerExact(false), m_deadzoneRadius(30.f), m_locked(Vec2::Zero), m_lockedObject(nullptr)
		{

		}

		constexpr float HalfTResX = TargetResolutionX / 2.f;
		constexpr float HalfTResY = TargetResolutionY / 2.f;


		void GameCamera::Update(Vec2 playerPos)
		{
			float deadzoneRadius = m_deadzoneRadius;

			if (m_locked.X != 0 || m_locked.Y != 0)
			{
				playerPos = m_locked;

				//Increase the accuracy of the camera centering lerp when approaching a lock position.
				deadzoneRadius /= 2;
			}


			if (m_lockedObject != nullptr)
			{
				playerPos = m_lockedObject->GetPosition();
				
				//Increase the accuracy of the camera centering lerp when approaching a lock position.
				deadzoneRadius /= 2;
			}

#if _DEBUG
			if (InputManager::GetKey(GLFW_KEY_L))
			{
				SetTargetZoom(1.f);
			}

			if (InputManager::GetKey(GLFW_KEY_K))
			{
				SetTargetZoom(0.25f);
			}
#endif

			/*if (m_followPlayerExact)
			{
				//SetPosition(Vec2(playerPos.X - HalfTResX, playerPos.Y - HalfTResY));
				//return;
			}*/

			const double sdt = GAME_NAME::Utils::Time::GameTime::GetScaledDeltaTime();

			if (m_targetZoom > 0)
			{
				//Lerp zoom.
				SetZoom(std::lerp(m_zoom, m_targetZoom, (float)CameraSpeed / 3.f * (float)sdt));
			}

			float resX = HalfTResX * (1 / m_zoom);
			float resY = HalfTResY * (1 / m_zoom);


			if (m_screenShakeMaxTimer > 0.0)
			{
				m_screenShakeTimer += sdt;
				m_screenShakeElapsed += sdt;

				if (m_screenShakeTimer > 0.05)
				{
					m_screenShakeOffset = -m_screenShakeOffset;
					m_screenShakeTimer = 0.0;
				}

				if (m_screenShakeElapsed >= m_screenShakeMaxTimer)
				{
					//Disable screen shaking.
					ScreenShake(0.f, 0.0);
				}
			}

			if (deadzoneRadius > 0.f && m_screenShakeElapsed <= 0.0)
			{
				//Deadzone
				if (m_useStrictFollowing)
				{

					if (Vec2::Distance(playerPos, m_position + Vec2{ HalfTResX, HalfTResY }) < 25.f)
					{
						return;
					}
				}
				else {
					if (!(playerPos.X + m_offset.X > m_position.X + resX + deadzoneRadius || playerPos.X + m_offset.X < m_position.X + resX - deadzoneRadius))
					{
						return;

					}
				}
				
			}


			float deadzoneXAdj = (playerPos.X > m_position.X + resX ? -deadzoneRadius : deadzoneRadius);
			float deadzoneYAdj = (playerPos.Y > m_position.Y + resY ? -deadzoneRadius : deadzoneRadius);

			Vec2 l = Vec2(std::lerp(m_position.X + resX, playerPos.X + m_offset.X + deadzoneXAdj, (float)(CameraSpeed * sdt)) - resX, std::lerp(m_position.Y + resY, playerPos.Y + m_offset.Y + deadzoneYAdj, m_screenShakeElapsed > 0.0 ? 1.0 : (float)(CameraSpeed * sdt)) - resY);

			SetPosition(l + Vec2{ m_screenShakeOffset, (m_screenShakeOffset > 0.f ? ((float)std::rand() / (float)RAND_MAX) : 0.f) });
		}

	}
}