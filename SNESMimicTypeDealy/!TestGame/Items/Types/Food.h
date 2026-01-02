#pragma once

#include "../InventoryItem.h"
#include "../../../Objects/MiscState.h"

#define ITEM_PREFIX_FOOD 'f'

namespace GAME_NAME::Items
{
	class Food
		: public InventoryItem
	{
	public:
		Food()
			: m_health(0)
		{

		}

		Food(ITEM_TYPE type, int health);

		///Eats the food.
		bool Use();

		SaveParam Encode() override final;
		void Decode(const SaveParam params) override final;

		std::unique_ptr<InventoryItem> GetUnique() override;
	protected:
		int m_health;

		inline constexpr const char getPrefix() override { return ITEM_PREFIX_FOOD; }
	};
}