#pragma once

#include "../InventoryItem.h"
#include "../../../Objects/MiscState.h"
#include "../../../Objects/MiscStateGroup.h"
#include <vector>

#define ITEM_PREFIX_PLACEABLE 'p'
#define PLACEABLE_STATE_GROUP_FILE "placed"

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
		void PlaceExact(Vec2 position);

		static void LoadPlaceables();

		std::unique_ptr<InventoryItem> GetUnique() override;
	protected:
		inline constexpr const char getPrefix() override { return ITEM_PREFIX_PLACEABLE; }

	private:

		class PlacedPlaceable
			: public MiscState
		{
		public:
			Vec2 Position = Vec2(0, 0);
			ITEM_TYPE Type = ITEM_TYPE::NULL_ITEM;
			std::string Level = "";

			PlacedPlaceable() {}
			
			PlacedPlaceable(Vec2 position, ITEM_TYPE item, std::string level)
				: Position(position), Type(item), Level(level)
			{

			}

			void Decode(const SaveParam params) override;
			SaveParam Encode() override;

			void Place();
		};

		class PlaceableStateGroup
			: public MiscStateGroup
		{
		public:
			PlaceableStateGroup()
				: MiscStateGroup(PLACEABLE_STATE_GROUP_FILE)
			{

			}

			void AddState(PlacedPlaceable* placeable);
			void LoadStates();
		};

		static PlaceableStateGroup* m_placedObjects;
	};
}