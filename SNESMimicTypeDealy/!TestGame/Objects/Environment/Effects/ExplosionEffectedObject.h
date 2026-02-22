#pragma once

#include "../../../../Utils/Math/Vec2.h"

namespace GAME_NAME::Objects::Environment::Effects
{
using namespace MathUtils;

	/// <summary>
	/// An object that reacts to nearby explosions.
	/// </summary>
	__interface ExplosionEffectedObject
	{
	public:
		/// <summary>
		/// Called when an explosion has occured in range of this object.
		/// </summary>
		/// <param name="origin">[Vec2] - origin of the explosion.</param>
		/// <param name="power">[float] - power of the explosion (how much damage it will do if you were at the center).</param>
		virtual void Exploded(Vec2 origin, float power) = 0;
	};
}