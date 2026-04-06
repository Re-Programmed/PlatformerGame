#pragma once

#include "../../../Utils/Math/Vec2.h"
#include "./AnimatingCharacter.h"

namespace GAME_NAME::Cutscenes
{
using namespace MathUtils;

	__interface ICharacterNode
	{
	public:
		const Vec2& GetPosition();
		bool Use(AnimatingCharacter* character);
	}; 
}