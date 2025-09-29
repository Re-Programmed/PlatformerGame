#pragma once

#include "../InventoryItem.h"
#include "../../../Objects/MiscState.h"

#define ITEM_PREFIX_PLACEABLE 'p'

namespace GAME_NAME::Items
{
	class Placeable
		: public InventoryItem
	{
	public:
		static double InteractionTimer; //Completed at 1.0.

		Placeable()
		{

		}

		Placeable(ITEM_TYPE type);

		Vec2 GetPlaceScale();

		Vec2 GetPotentialPlaceLocation(Vec2& playerPosition);
		void RenderPreview(const Vec2& cameraPosition, const Vec2& position);

		void Place(Vec2 playerPosition);

		SaveParam Encode() override final;
		void Decode(const SaveParam params) override final;
	protected:
		inline constexpr const char getPrefix() override { return ITEM_PREFIX_PLACEABLE; }
	};
}