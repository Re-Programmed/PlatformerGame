#pragma once
#include "../../Objects/GameObject.h"

namespace GAME_NAME::Objects 
{
	class SoundMaterial
	{
	public:
		enum class Material
		{
			None,
			Grass,
			Wood
		};

		static Material GetSoundMaterial(GameObject* object);
	};
}