#pragma once
#include "../../../../Utils/Math/Vec2.h"
#include "../../../../Utils/ConstUpdateable.h"
#include <map>
#include "../../../../Objects/GameObject.h"

namespace GAME_NAME::Objects::Environment::Effects
{
	using namespace MathUtils;
	using namespace Utils;

	class ElectricalZap
		: public ConstUpdateable
	{
	public:
		static ElectricalZap* INSTANCE;

		ElectricalZap()
			: ConstUpdateable()
		{

		}

		static void CreateZapBetweenPoints(Vec2 origin, Vec2 destination);

		static inline void Init()
		{
			if (INSTANCE != nullptr)
			{
				return;
			}

			ElectricalZap* ez = new ElectricalZap();
			INSTANCE = ez;
		}

		void Update(GLFWwindow* window) override;

	private:
		static std::map<GameObject*, double> m_zapEffects;
	};


}