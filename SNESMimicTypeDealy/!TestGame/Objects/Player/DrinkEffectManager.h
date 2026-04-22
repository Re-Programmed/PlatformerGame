#pragma once

#include "../../Items/Types/Drink.h"
#include "../../../Objects/GUI/StaticGUIElement.h"

namespace GAME_NAME::Objects::Player
{
	class DrinkEffectManager
	{
	public:
		void ApplyEffect(GAME_NAME::Items::DrinkEffect effect);
		inline void ApplyEffect(GAME_NAME::Items::Drink* drink) { this->ApplyEffect(drink->GetEffect()); };

		void Update();

		int GetTotalEffect(GAME_NAME::Items::DrinkEffectType effectType);
	private:
		std::vector<std::tuple<GAME_NAME::Items::DrinkEffect, double, GUI::StaticGUIElement*>> m_appliedEffects;
	};
}