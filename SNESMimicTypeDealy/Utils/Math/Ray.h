#pragma once

#include "Vec2.h"
#include "../../Objects/GameObject.h"

namespace GAME_NAME::MathUtils
{

	class Ray
	{
	public:
		static Objects::GameObject* CollisionCast(Vec2 origin, bool left, float travelDistance, Objects::GameObject* immune = nullptr);

		/// <summary>
		/// Shoots out a raycast and returns the object that is hit.
		/// </summary>
		/// <param name="origin">[const Vec2] the initial point.</param>
		/// <param name="direction">[Vec2] where the ray is going.</param>
		/// <param name="maxDistance">[const float&&] the maximum distance that the ray is allowed to go.</param>
		/// <param name="immune">[GameObject*] an object that cannot be hit by the ray (ex. the player if they are using a weapon).</param>
		/// <returns>[GameObject*] The first object hit.</returns>
		static Objects::GameObject* Cast(const Vec2 origin, Vec2 direction, Vec2& hitPoint, const float&& maxDistance = 500.f, Objects::GameObject* immune = nullptr);

		/// <summary>
		/// Perform a raycast within a loop. (Good if you want a raycast that can hit multiple objects.
		/// </summary>
		/// <param name="origin">[const Vec2&] where the ray will begin.</param>
		/// <param name="direction">[Vec2] where the ray is going.</param>
		/// <param name="iterations">[int&] a variable to store how many iterations have occured. (Should be intilized at 0.)</param>
		/// <param name="hit">[GameObject*] an object that can store the currently intersected object.</param>
		/// <param name="maxDistance">[const float&&] the maximum distance that the ray is allowed to go.</param>
		/// <param name="immune">[GameObject*] an object that cannot be hit by the ray (ex. the player if they are usi</param>
		/// <param name="stopOnHit">[const bool&&] if the cast should stop upon hitting an object.</param>
		/// <returns>[bool] If the cast should continue.</returns>
		bool ContinuousCast(const Vec2& origin, Vec2& direction, int& iterations, Objects::GameObject*& hit, const float&& maxDistance = 500.f, Objects::GameObject* immune = nullptr, const bool&& stopOnHit = false);

		inline Vec2 GetCurrentPoint() { return m_continuousPoint; }

	private:
		/// <summary>
		/// Used to track the current position of a continuous cast.
		/// </summary>
		static Vec2 m_continuousPoint;
		/// <summary>
		/// Used to store the max iterations of a continuous cast so it doesn't have to recalculate every iteration.
		/// </summary>
		static int m_maxIterations;
	};
}