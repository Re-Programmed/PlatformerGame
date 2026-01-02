#include "Food.h"

#include "../../TestGame.h"

namespace GAME_NAME::Items
{
	Food::Food(ITEM_TYPE type, int health)
		: InventoryItem(type), m_health(health)
	{

	}

	MiscState::SaveParam Food::Encode()
	{
		MiscState::SaveParam param = InventoryItem::Encode();
		//f_(type)
		return param;
	}

	bool Food::Use()
	{
		TestGame::ThePlayer->Heal(m_health);
		return true;
	}

	void Food::Decode(const SaveParam params)
	{
		InventoryItem::Decode(params);
	}

	std::unique_ptr<InventoryItem> Food::GetUnique()
	{
		return std::unique_ptr<Food>(this);
	}

}