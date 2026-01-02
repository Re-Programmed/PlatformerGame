#pragma once

#include "../InventoryItem.h"
#include "../../../Objects/MiscState.h"

#define ITEM_PREFIX_WEAPON 'w'

namespace GAME_NAME::Items
{
	class Weapon
		: public InventoryItem
	{
	public:
		Weapon() = default;

		Weapon(ITEM_TYPE type, int uses);

		bool Use();

		int GetDamage();
		double GetCooldown();
		int GetPowerConsumption();

		SaveParam Encode() override final;
		void Decode(const SaveParam params) override final;

		std::unique_ptr<InventoryItem> GetUnique() override;
	protected:
		int m_uses SERIALIZED;

		inline constexpr const char getPrefix() override { return ITEM_PREFIX_WEAPON; }
	};
}