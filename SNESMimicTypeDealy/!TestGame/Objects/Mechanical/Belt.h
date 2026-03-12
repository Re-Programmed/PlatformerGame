#pragma once

#ifndef _BELT_CLASS
#define _BELT_CLASS

#include "../../../Objects/GameObject.h"
#include "./Cog.h"

namespace GAME_NAME::Objects::Mechanical
{
	class Cog;

	class Belt
		: public GameObject
	{
	public:
		Belt(Cog* firstConnection, Cog* secondConnection = nullptr);

		void Update(GLFWwindow* window) override;

		void Render(const Vec2& cameraPosition) override;

		inline const float& GetBeltSpeed() { return m_beltSpeed; }

		inline void SetFirstConnection(Cog* cog) { m_firstConnection = cog; }
		inline void SetSecondConnection(Cog* cog) { m_secondConnection = cog; }

		void RemoveConnection(Cog* cog);

	private:
		float m_beltSpeed;

		Cog* m_firstConnection, *m_secondConnection;

	};
}

#endif