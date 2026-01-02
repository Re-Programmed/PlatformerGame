#pragma once

#include "../InventoryItem.h"
#include "../../../Objects/MiscState.h"

#define ITEM_PREFIX_TOOL 't'

namespace GAME_NAME::Items
{
	class Tool
		: public InventoryItem
	{
	public:
		Tool() = default;

		Tool(ITEM_TYPE type, int uses);

		/// <summary>
		/// Decrease the number of uses left.
		/// </summary>
		/// <returns>True if the tool broke.</returns>
		bool Use();

		SaveParam Encode() override final;
		void Decode(const SaveParam params) override final;

		std::unique_ptr<InventoryItem> GetUnique() override;
	protected:
		int m_uses SERIALIZED;

		inline constexpr const char getPrefix() override { return ITEM_PREFIX_TOOL; }
	};
}