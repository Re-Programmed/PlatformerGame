#pragma once

#include "../../../Objects/MiscStateGroup.h"
#include <unordered_map>

namespace GAME_NAME::Objects::Collectables
{
	class CollectableTracker
		: public MiscStateGroup
	{
	public:
		static CollectableTracker* INSTANCE;

		CollectableTracker()
			: MiscStateGroup("collect")
		{
			if (INSTANCE != nullptr) {
				return;
			}

			INSTANCE = this;

			auto states = getStates();

			if (states->size() > 0)
			{
				CurrentCollectableData->Decode((*states)[0]);
			}

			this->assignState(CurrentCollectableData);
		}

		/// <summary>
		/// Types of collectables.
		/// </summary>
		enum COLLECTABLE_TYPE
		{
			TOAST,
			CRUMB
		};

		~CollectableTracker()
		{
			delete CurrentCollectableData;
		}

		static void Init()
		{
			if (INSTANCE == nullptr) { INSTANCE = new CollectableTracker(); }
		}

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
		/// Should be called upon completing a level to update the player's currency stats.
		/// </summary>
		static void UpdateCurrencyAtEnd();

		/// <summary>
		/// Add value to some type of collectable.
		/// </summary>
		static void AddCollectable(COLLECTABLE_TYPE type);
	};
}