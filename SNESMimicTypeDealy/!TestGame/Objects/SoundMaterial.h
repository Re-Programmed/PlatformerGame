#pragma once
#include "../../Objects/GameObject.h"

#include "../../Audio/SoundEvents.h"

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

		static Audio::SoundEvents::Event GetWalkEffectForMaterial(Material material);
		static Audio::SoundEvents::Event GetHitEffectForMaterial(Material material);
	};
}