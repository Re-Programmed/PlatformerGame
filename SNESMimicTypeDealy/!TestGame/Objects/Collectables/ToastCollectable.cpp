#include "ToastCollectable.h"

#include "CollectableTracker.h"

#define TOAST_COLLECTABLE_SPRITE SpriteBase(132)

namespace GAME_NAME::Objects::Collectables
{
	ToastCollectable::ToastCollectable(Vec2 position, Vec2 scale, uint8_t toastID, size_t saveID)
		: BasicCollectable(position, scale, Renderer::GetSprite(TOAST_COLLECTABLE_SPRITE), saveID), m_toastID(toastID)
	{

		//If the toast was collected in a previous playthrough of the level, the final result of toast collected still needs to be updated.
		if (m_isCollected)
		{
			GlobalLevelData::CollectToast(m_toastID);
		}
	}

	void ToastCollectable::onCollect()
	{
		GlobalLevelData::CollectToast(m_toastID);
		CollectableTracker::AddCollectable(CollectableTracker::COLLECTABLE_TYPE::TOAST);
	}
}

