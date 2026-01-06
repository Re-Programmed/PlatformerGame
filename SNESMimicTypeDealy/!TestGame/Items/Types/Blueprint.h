#pragma once

#include "../InventoryItem.h"
#include "../../../Objects/MiscState.h"

#define ITEM_PREFIX_BLUEPRINT 'b'

namespace GAME_NAME::Items
{
	class Blueprint
		: public InventoryItem
	{
	public:
		Blueprint()
			: m_recipeID(0)
		{

		}

		Blueprint(ITEM_TYPE type, int recipeID);

		///Unlocks the recipe.
		bool Use();

		SaveParam Encode() override final;
		void Decode(const SaveParam params) override final;

		std::unique_ptr<InventoryItem> GetUnique() override;
	protected:
		int m_recipeID;

		inline constexpr const char getPrefix() override { return ITEM_PREFIX_BLUEPRINT; }
	};
}