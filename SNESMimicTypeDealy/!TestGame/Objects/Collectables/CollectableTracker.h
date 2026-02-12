#pragma once

#include "../../../Objects/MiscStateGroup.h"
#include <unordered_map>

namespace GAME_NAME::Objects::Collectables
{
	class CollectableTracker
		: public MiscStateGroup
	{
	public:
		/// <summary>
		/// Types of collectables.
		/// </summary>
		enum COLLECTABLE_TYPE
		{
			TOAST,
			CRUMB
		};

		/// <summary>
		/// Stores the save data for what collectables have been gotten.
		/// </summary>
		class CollectableData
			: public MiscState
		{
		public:
			std::unordered_map<int, int> CollectionData;

			void Decode(const SaveParam params) override;
			SaveParam Encode() override;
		};

		/// <summary>
		/// Current save state for collected collectables.
		/// </summary>
		static CollectableData* const CurrentCollectableData;

		/// <summary>
		/// Add value to some type of collectable.
		/// </summary>
		static void AddCollectable(COLLECTABLE_TYPE type);
	};
}