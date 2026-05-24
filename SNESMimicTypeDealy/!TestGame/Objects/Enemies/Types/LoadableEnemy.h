#pragma once
#include "../../../../Utils/Math/Vec2.h"

namespace GAME_NAME::Objects::Enemies
{
	class LoadableEnemy
	{
	public:
		enum class LoadMode
		{
			Initilize,
			WhenOffscreen,
			OnZoneEntered
		};

		LoadableEnemy(LoadMode mode)
			: m_mode(mode)
		{
			if (mode == LoadMode::Initilize)
			{
				load();
				m_hasLoaded = true;
			}
			else {
				unload();
			}
		}

		void Update(MathUtils::Vec2 position, MathUtils::Vec2 scale);

	protected:
		virtual void load()
		{

		}

		virtual void unload()
		{

		}

		bool m_hasLoaded = false;

		inline const LoadMode& getLoadMode() { return m_mode; }
	private:
		const LoadMode m_mode;

	};
}