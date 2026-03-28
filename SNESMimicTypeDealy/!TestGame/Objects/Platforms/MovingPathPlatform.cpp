#include "MovingPathPlatform.h"
#include <stdarg.h>
#include "../../../Rendering/DynamicSprite.h"
#include "../../../Rendering/Renderers/Renderer.h"

#define MOVING_PATH_PLATFORM_PATH_TEXTURE SpriteBase(439)
#define MOVING_PATH_PLATFORM_JOINT_TEXTURE SpriteBase(440)

#define MOVING_PATH_PLATFORM_JOINT_DIAMETER 5.f

#define MOVING_PATH_PLATFORM_PATH_WIDTH 16.f
#define MOVING_PATH_PLATFORM_PATH_HEIGHT 3.f

namespace GAME_NAME::Objects::Platforms
{
	MovingPathPlatform::MovingPathPlatform(Vec2 startPosition, Vec2 scale, Rendering::Sprite* sprite, bool requiresTrigger, float speed, uint8_t pointCount, ...)
		: BasicMovingPlatform(startPosition - scale/2.f, scale, sprite), m_speed(requiresTrigger ? 0.f : speed), m_normalSpeed(speed), m_requiresTrigger(requiresTrigger)
	{
		va_list args;
		va_start(args, pointCount);

		m_motionPoints.emplace_back(startPosition);

		for (int i = 0; i < pointCount; i++)
		{
			m_motionPoints.emplace_back(va_arg(args, Vec2));
		}
	}

#define MOVING_PATH_PLATFORM_TOLERENCE 0.6f

	void MovingPathPlatform::Update(GLFWwindow* window)
	{
		//Call first to update the lastposition variable that moves objects on top of the platform.
		BasicMovingPlatform::Update(window);

		Vec2 motionAnchor = m_position + m_scale / 2.f;	//The center of the object.
		Vec2 targetPoint(m_speed < 0.f ? m_motionPoints[m_currentInterval] : m_motionPoints[m_currentInterval + 1]);	//If we are moving fowrad, use the next point, otherwise use the previous point.
		Vec2 lastPoint(m_speed < 0.f ? m_motionPoints[m_currentInterval + 1] : m_motionPoints[m_currentInterval]);

		if (m_speed == 0.f) { return; }

		//Move platform some constant rate towards the target point.
		if (Vec2::Distance(motionAnchor, targetPoint) >= MOVING_PATH_PLATFORM_TOLERENCE)
		{
			float dt(static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()));
			Vec2 d(((targetPoint.X - motionAnchor.X) < 0.f ? -1.f : 1.f) * dt * std::abs(m_speed), ((targetPoint.Y - motionAnchor.Y) < 0.f ? -1.f : 1.f) * dt * std::abs(m_speed));

			//Fix float math.
			if (std::abs(targetPoint.X - lastPoint.X) < 1.5f) { d.X = 0.f; }
			if (std::abs(targetPoint.Y - lastPoint.Y) < 1.5f) { d.Y = 0.f; }

			Translate(d);
		}
		else {
			if (m_speed > 0.f)
			{
				m_currentInterval++;
				if (m_currentInterval >= m_motionPoints.size() - 1)
				{
					//Flip direction of motion.
					m_currentInterval--;
					
					if(!m_requiresTrigger){ m_speed *= -1.f; }
				}
			}
			else if(m_speed < 0.f) {
				if (m_currentInterval <= 0)
				{
					//Flip direction of motion.
					m_currentInterval = 0;
					if (!m_requiresTrigger) { m_speed *= -1.f; }
				}
				else {
					m_currentInterval--;
				}
			}
		}

	}

	void MovingPathPlatform::Render(const Vec2& cameraPos)
	{
		DrawPath(cameraPos, m_motionPoints);

		BasicMovingPlatform::Render(cameraPos);
	}

	void MovingPathPlatform::AddMotionPoint(Vec2 point)
	{
		m_motionPoints.emplace_back(point);
	}

	void MovingPathPlatform::DrawPath(const Vec2& cameraPos, size_t pointCount, ...)
	{
		va_list args;
		va_start(args, pointCount);

		Vec2 lastPoint(va_arg(args, Vec2));

		DynamicSprite jointSprite(Renderer::GetSpriteIdFromTextureId(MOVING_PATH_PLATFORM_JOINT_TEXTURE));
		for (int i = 0; i < pointCount; i++)
		{
			//Render a joint at each point on the path.
			jointSprite.Render(cameraPos, Vec2{ lastPoint.X - MOVING_PATH_PLATFORM_JOINT_DIAMETER / 2.f, lastPoint.Y - MOVING_PATH_PLATFORM_JOINT_DIAMETER / 2.f }, MOVING_PATH_PLATFORM_JOINT_DIAMETER);
			if (i >= pointCount - 1) { break; }

			Vec2 nextPoint(va_arg(args, Vec2));
			drawConnectedPoints(cameraPos, lastPoint, nextPoint);
			lastPoint = nextPoint;
		}
	}

	void MovingPathPlatform::DrawPath(const Vec2& cameraPos, const std::vector<Vec2>& points)
	{
		DynamicSprite jointSprite(Renderer::GetSpriteIdFromTextureId(MOVING_PATH_PLATFORM_JOINT_TEXTURE));
		for (int i = 0; i < points.size() - 1; i++)
		{
			drawConnectedPoints(cameraPos, points[i], points[i + 1]);
		}

		for (int i = 0; i < points.size(); i++)
		{
			//Render a joint at each point on the path.
			jointSprite.Render(cameraPos, Vec2{ points[i].X - MOVING_PATH_PLATFORM_JOINT_DIAMETER / 2.f, points[i].Y - MOVING_PATH_PLATFORM_JOINT_DIAMETER / 2.f }, MOVING_PATH_PLATFORM_JOINT_DIAMETER);
		}
	}

	void MovingPathPlatform::drawConnectedPoints(const Vec2& cameraPos, Vec2 point1, Vec2 point2)
	{
using namespace Rendering;

		bool completedPath = false;
		DynamicSprite sprite(Renderer::GetSpriteIdFromTextureId(MOVING_PATH_PLATFORM_PATH_TEXTURE));

		//Loop to tile the path texture.
		while (!completedPath)
		{
			//Calculate how much the next path piece is supposed to represent.
			float dx(point2.X - point1.X);
			float dy(point2.Y - point1.Y);

			Vec2 d(0.f);

			//Make sure the platform only moves along 1 axis.
			if (std::abs(dx) > std::abs(dy))
			{
				d = Vec2{ dx, 0.f };
			}
			else {
				d = Vec2{ 0.f, dy };
			}

			//If the platform path is going to render vertically, the texture verticies are flipped to make it look nice.
			if (d.Y != 0.f)
			{
				Vec2 texVerts[4] = {
					{ 1.f, 1.f },
					{ 0.f, 1.0f },
					{ 0.0f, 0.0f },
					{ 1.f, 0.f }
				};

				sprite.UpdateTextureVerticies(texVerts);
			}
			else {
				Vec2 texVerts[4] = {
					{ 1.f, 1.0f },
					{ 1.f, 0.0f },
					{ 0.0f, 0.0f },
					{ 0.0f, 1.0f }
				};

				sprite.UpdateTextureVerticies(texVerts);
			}

			//Ensure that the path only extends by one width, tiles textures rather than stretching them.
			if (d.X > MOVING_PATH_PLATFORM_PATH_WIDTH) { d.X = MOVING_PATH_PLATFORM_PATH_WIDTH; }
			else if (d.X < -MOVING_PATH_PLATFORM_PATH_WIDTH) { d.X = -MOVING_PATH_PLATFORM_PATH_WIDTH; }
			else if (d.Y > MOVING_PATH_PLATFORM_PATH_WIDTH) { d.Y = MOVING_PATH_PLATFORM_PATH_WIDTH; }
			else if (d.Y < -MOVING_PATH_PLATFORM_PATH_WIDTH) { d.Y = -MOVING_PATH_PLATFORM_PATH_WIDTH; }
			else { 
				//If the next platform path piece is supposed to be shorter than the width of a normal piece, cut the texture short so it looks nice.

				if (d.Y == 0.f)
				{
					Vec2 texVerts[4] = {
						{ d.X / MOVING_PATH_PLATFORM_PATH_WIDTH, 1.0f },
						{ d.X / MOVING_PATH_PLATFORM_PATH_WIDTH, 0.0f },
						{ 0.0f, 0.0f },
						{ 0.0f, 1.0f }
					};

					sprite.UpdateTextureVerticies(texVerts);
				}
				else {
					Vec2 texVerts[4] = {
						{ d.Y / MOVING_PATH_PLATFORM_PATH_WIDTH, 1.f },
						{ 0.f, 1.0f },
						{ 0.0f, 0.0f },
						{ d.Y / MOVING_PATH_PLATFORM_PATH_WIDTH, 0.f }
					};

					sprite.UpdateTextureVerticies(texVerts);
				}

				completedPath = true;
			}

			//Render the texture either vertically or horizontally.
			if (d.Y == 0.f)
			{
				sprite.Render(cameraPos, point1 - Vec2{ 0.f, MOVING_PATH_PLATFORM_PATH_HEIGHT/2.f }, Vec2{ d.X, MOVING_PATH_PLATFORM_PATH_HEIGHT });
			}
			else {
				sprite.Render(cameraPos, point1 - Vec2{ MOVING_PATH_PLATFORM_PATH_HEIGHT/2.f, 0.f }, Vec2{ MOVING_PATH_PLATFORM_PATH_HEIGHT, d.Y });
			}

			//Move to next point.
			point1 += d;
		}
	}

	void MovingPathPlatform::Trigger(bool reverse, float intensity)
	{
		if (!m_requiresTrigger) { return; }

		m_speed = (reverse ? -1.f : 1.f) * m_normalSpeed * intensity;
	}

}
