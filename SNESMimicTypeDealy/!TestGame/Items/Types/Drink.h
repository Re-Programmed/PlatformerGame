#pragma once
#include "../InventoryItem.h"

#define ITEM_PREFIX_DRINK 'd'

namespace GAME_NAME::Items
{
	enum class DrinkEffectType
	{
		STRENGTH
	};

	typedef struct DrinkEffect
	{
		DrinkEffectType Type;
		int Amplifier;
		double Duration;

		GLuint Icon;
	};

	class Drink
		: public InventoryItem
	{
	public:
		DrinkEffect GetEffect();

	protected:
		inline constexpr const char getPrefix() override { return ITEM_PREFIX_DRINK; }
		

	};
}