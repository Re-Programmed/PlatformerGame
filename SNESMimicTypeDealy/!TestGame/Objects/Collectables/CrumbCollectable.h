#pragma once

#include "BasicCollectable.h"

namespace GAME_NAME::Objects::Collectables
{
	class CrumbCollectable
		: public BasicCollectable
	{
	public:

		CrumbCollectable(Vec2 position, Vec2 scale, size_t saveID);
	protected:
		void onCollect() override;
	};
}