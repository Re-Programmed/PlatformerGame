#include "DrinkEffectManager.h"

#include "../../../Utils/Time/GameTime.h"

namespace GAME_NAME::Objects::Player
{
using namespace GAME_NAME::Items;

	void DrinkEffectManager::ApplyEffect(DrinkEffect effect)
	{
		GUI::StaticGUIElement* icon = nullptr;

		//Create icon display.
		if (effect.Icon != 0)
		{
			icon = new GUI::StaticGUIElement(Vec2{ TargetResolutionX - 20.f - (m_appliedEffects.size() * 20.f), TargetResolutionY - 20.f}, Vec2{16.f, 16.f}, effect.Icon);

			Renderer::LoadGUIElement(icon);
		}

		m_appliedEffects.emplace_back(effect, 0.0, icon);


	}

	void DrinkEffectManager::Update()
	{
		for (int i = 0; i < m_appliedEffects.size(); i++)
		{
			if (std::get<double>(m_appliedEffects[i]) < std::get<DrinkEffect>(m_appliedEffects[i]).Duration)
			{
				std::get<double>(m_appliedEffects[i]) += Utils::Time::GameTime::DeltaTime::GetDeltaTime();

				std::get<GUI::StaticGUIElement*>(m_appliedEffects[i])->SetPosition(Vec2{ TargetResolutionX - (i + 1) * 20.f, TargetResolutionY - 20.f });
			}
		}

		std::erase_if(m_appliedEffects, [](const auto& t) { 
			bool shouldErase = std::get<double>(t) >= std::get<DrinkEffect>(t).Duration;
			
			//Remove icon display.
			if (shouldErase)
			{
				Renderer::UnloadGUIElement(std::get<GUI::StaticGUIElement*>(t));
				delete std::get<GUI::StaticGUIElement*>(t);
			}

			return shouldErase;
		});
	}

	int DrinkEffectManager::GetTotalEffect(DrinkEffectType effectType)
	{
		int sum = 0;

		for (int i = 0; i < m_appliedEffects.size(); i++)
		{
			if (std::get<DrinkEffect>(m_appliedEffects[i]).Type == effectType)
			{
				sum += std::get<DrinkEffect>(m_appliedEffects[i]).Amplifier;
			}
		}

		return sum;
	}
}

