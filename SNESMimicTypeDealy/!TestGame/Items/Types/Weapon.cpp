#include "Weapon.h"

namespace GAME_NAME::Items
{
	Weapon::Weapon(ITEM_TYPE type, int uses)
		: InventoryItem(type), m_uses(uses)
	{

	}

	bool Weapon::Use()
	{
		//std::cout << "YOU USED THE " << ITEMTYPE_GetItemTypeName(m_itemType) << std::endl;
		return false;
	}

	MiscState::SaveParam Weapon::Encode()
	{
		MiscState::SaveParam param = InventoryItem::Encode();

		param += std::string("+").append(std::to_string(m_uses));

		//w_(type)+(uses)
		return param;
	}

	int Weapon::GetDamage()
	{
		std::string data = ITEM_DATA[m_itemType].Attributes.at(WEAPON);
		return std::stoi(data.substr(0, data.find_first_of(',')));
	}

	int Weapon::GetPowerConsumption()
	{
		std::string data = ITEM_DATA[m_itemType].Attributes.at(WEAPON);
		return std::stod(data.substr(data.find_first_of(',') + 1, data.find_last_of(',')));
	}

	double Weapon::GetCooldown()
	{
		std::string data = ITEM_DATA[m_itemType].Attributes.at(WEAPON);
		return std::stoi(data.substr(data.find_last_of(',') + 1));
	}

	void Weapon::Decode(const SaveParam params)
	{
		//Decode uses.
		m_uses = std::stoi(params.substr(params.find('+') + 1));

		//Call base decoding.
		InventoryItem::Decode(ITEM_PREFIX_ITEM + params.substr(1, params.find('+') - 1));
	}
	std::unique_ptr<InventoryItem> Weapon::GetUnique()
	{
		return std::unique_ptr<Weapon>(this);
	}
}