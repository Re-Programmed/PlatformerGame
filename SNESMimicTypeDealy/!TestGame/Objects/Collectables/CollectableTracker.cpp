#include "CollectableTracker.h"

#include <sstream>

#include "../../Objects/Player/Currency.h"

namespace GAME_NAME::Objects::Collectables
{
	CollectableTracker* CollectableTracker::INSTANCE = nullptr;

	CollectableTracker::CollectableData* const CollectableTracker::CurrentCollectableData = new CollectableTracker::CollectableData();

	void CollectableTracker::CollectableData::Decode(const SaveParam params)
	{
		std::stringstream paramsIterator(params);
		std::string param;

		int lastKey = -1;
		while (std::getline(paramsIterator, param, ':'))
		{
			if (lastKey != -1)
			{
				CollectionData.emplace(std::make_pair(lastKey, std::stoi(param)));
				lastKey = -1;
				continue;
			}

			lastKey = std::stoi(param);
		}
	}

	CollectableTracker::CollectableData::SaveParam CollectableTracker::CollectableData::Encode()
	{
		SaveParam result;

		for (auto collectable : this->CollectionData)
		{
			result.append(std::to_string(collectable.first));
			result.append(":");
			result.append(std::to_string(collectable.second));
			result.append(":");
		}

		return result;
	}


	void CollectableTracker::UpdateCurrencyAtEnd()
	{
		Player::Currency::AddCrumbs(CurrentCollectableData->CollectionData.at(COLLECTABLE_TYPE::CRUMB));
		Player::Currency::AddToast(CurrentCollectableData->CollectionData.at(COLLECTABLE_TYPE::TOAST));

		CurrentCollectableData->CollectionData[COLLECTABLE_TYPE::CRUMB] = 0;
		CurrentCollectableData->CollectionData[COLLECTABLE_TYPE::TOAST] = 0;
	}

	void CollectableTracker::AddCollectable(COLLECTABLE_TYPE type)
	{
		if (CurrentCollectableData->CollectionData.contains(type))
		{
			CurrentCollectableData->CollectionData[type] += 1;
		}else{
			CurrentCollectableData->CollectionData.emplace((int)type, 1);
		}


	}

}

