#pragma once

namespace GAME_NAME::Objects
{
	class Triggerable
	{
	public:
		/// <summary>
		/// Called every frame something is interacting with this triggerable.
		/// </summary>
		/// <param name="reverse"></param>
		virtual void Trigger(bool reverse = false, float intensity = 1.f) = 0;
	};
}