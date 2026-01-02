#include "Currency.h"
#include "../../../Resources/Save/SaveManager.h"
#include "../../../Objects/GUI/Text/TextRenderer.h"

#include "../../Items/ItemType.h"

#define CRUMB_COUNTER_POSITION Vec2{ 10.f, TargetResolutionY - 24.f }
#define CRUMB_COUNTER_SCALE 0.33f

namespace GAME_NAME::Objects::Player
{
using namespace Resources;

	constexpr const char* CURRENCY_CRUMB_COUNT_SAVE_STRING = "CrumbCount";

	uint16_t Currency::m_currentCrumbCount = 0;

	std::array<std::unique_ptr<GUI::StaticGUIElement>, 4> Currency::m_currencyDisplay;

	void Currency::AddCrumbs(int16_t amount)
	{
		//TODO: Play cha-ching noise.
		m_currentCrumbCount += amount;
		Save();

		//If the currency display exists, update it.
		if (m_currencyDisplay[0] != nullptr)
		{
			RenderCurrencyDisplay();
		}
	}

	bool Currency::RemoveCrumbs(int16_t amount)
	{
		if (m_currentCrumbCount - amount < 0)
		{
			return false;
		}

		m_currentCrumbCount -= amount;
		Save();

		//If the currency display exists, update it.
		if (m_currencyDisplay[0] != nullptr)
		{
			RenderCurrencyDisplay();
		}

		return true;
	}

	void Currency::RenderCurrencyDisplay()
	{
		for (int i = 0; i < m_currencyDisplay.size(); i++)
		{
			if (m_currencyDisplay[i] != nullptr)
			{
				Renderer::UnloadGUIElement(m_currencyDisplay[i].get());
			}
		}

		Vec2 pos = CRUMB_COUNTER_POSITION;
		GUI::Text::TextRenderer::RenderedDigit count = GUI::Text::TextRenderer::RenderNumber(m_currentCrumbCount, pos, CRUMB_COUNTER_SCALE, 0.f, 3);

		m_currencyDisplay[0].reset(count[0]);
		m_currencyDisplay[1].reset(count[1]);
		m_currencyDisplay[2].reset(count[2]);

		Sprite* sp = Items::ITEMTYPE_GetItemTypeTexture(Items::CRUMBS);
		GUI::StaticGUIElement* crumbImage = new GUI::StaticGUIElement(pos + Vec2{ -count[0]->GetScale().X * 2.f, 0.f }, Vec2{ -count[0]->GetScale().X }, sp->GetSpriteId());
		delete sp;
		Renderer::LoadGUIElement(crumbImage);

		m_currencyDisplay[3].reset(crumbImage);
	}

	void Currency::Load()
	{
		int value = 0;
		SaveManager::GetSaveInteger(CURRENCY_CRUMB_COUNT_SAVE_STRING, value);
		m_currentCrumbCount = value;
	}

	void Currency::Save()
	{
		SaveManager::SaveInteger(m_currentCrumbCount, CURRENCY_CRUMB_COUNT_SAVE_STRING);
	}
}