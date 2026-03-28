#pragma once

#include "../../../Components/Physics/Collision/Helpers/BasicMovingPlatform.h"

#include "../Triggerable.h"

namespace GAME_NAME::Objects::Platforms
{

using namespace GAME_NAME::Components::Physics::Collision;
using namespace GAME_NAME::MathUtils;

	class MovingPathPlatform
		: public BasicMovingPlatform, public Triggerable
	{

	public:
		MovingPathPlatform(Vec2 startPosition, Vec2 scale, Rendering::Sprite* sprite, bool requiresTrigger, float speed, uint8_t pointCount, ...);

		void Update(GLFWwindow* window) override;

		void Render(const Vec2& cameraPos) override;

		void AddMotionPoint(Vec2 point);

		void Trigger(bool reverse = false, float intensity = 1.f);

		static void DrawPath(const Vec2& cameraPos, size_t pointCount, ...);
		static void DrawPath(const Vec2& cameraPos, const std::vector<Vec2>& points);
	protected:
		size_t m_currentInterval = 0;
		std::vector<Vec2> m_motionPoints;
		const bool m_requiresTrigger;

		float m_speed = 1.f;
		/// <summary>
		/// If this requires a trigger, used to track the desired speed.
		/// </summary>
		const float m_normalSpeed;

	private:

		static void drawConnectedPoints(const Vec2& cameraPos, Vec2 point1, Vec2 point2);
	};
}