#include "CrumbCollectable.h"

#include "CollectableTracker.h"

#define CRUMB_COLLECTABLE_SPRITE SpriteBase(257)

namespace GAME_NAME::Objects::Collectables
{

	CrumbCollectable::CrumbCollectable(Vec2 position, Vec2 scale, size_t saveID)
		: BasicCollectable(position, scale, Renderer::GetSprite(CRUMB_COLLECTABLE_SPRITE), saveID)
	{

	}

	void CrumbCollectable::onCollect()
	{
		CollectableTracker::AddCollectable(CollectableTracker::COLLECTABLE_TYPE::CRUMB);
	}

}
