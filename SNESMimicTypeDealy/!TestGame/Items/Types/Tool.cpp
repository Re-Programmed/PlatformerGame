#include "Tool.h"

namespace GAME_NAME::Items
{
	Tool::Tool(ITEM_TYPE type, int uses)
		: InventoryItem(type), m_uses(uses)
	{

	}

	MiscState::SaveParam Tool::Encode()
	{
		MiscState::SaveParam param = InventoryItem::Encode();

		param += std::string("+").append(std::to_string(m_uses));

		//t_(type)+(uses)
		return param;
	}

	bool Tool::Use()
	{
		m_uses--;

		return m_uses < 1;
	}

	void Tool::Decode(const SaveParam params)
	{
		//Decode uses.
		m_uses = std::stoi(params.substr(params.find('+') + 1));

		//Call base decoding.
		InventoryItem::Decode(ITEM_PREFIX_ITEM + params.substr(1, params.find('+') - 1));
	}

	std::unique_ptr<InventoryItem> Tool::GetUnique()
	{
		return std::unique_ptr<Tool>(this);
	}

}