#include "LoadableEnemy.h"

#include "../../../TestGame.h"

namespace GAME_NAME::Objects::Enemies
{
	void LoadableEnemy::Update(MathUtils::Vec2 position, MathUtils::Vec2 scale)
	{
		if (m_hasLoaded) { return; }

		if (m_mode == LoadMode::WhenOffscreen)
		{
			if (TestGame::INSTANCE->GetCamera()->GetPosition().X > position.X + scale.X)
			{
				load();
				m_hasLoaded = true;
			}
		}
	}
}