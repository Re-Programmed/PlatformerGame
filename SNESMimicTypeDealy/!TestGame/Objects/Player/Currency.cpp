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
	constexpr const char* CURRENCY_TOAST_COUNT_SAVE_STRING = "ToastCount";

	uint16_t Currency::m_currentCrumbCount = 0;
	uint16_t Currency::m_currentToastCount = 0;

	std::array<std::unique_ptr<GUI::StaticGUIElement>, 7> Currency::m_currencyDisplay;

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

	void Currency::AddToast(int16_t amount)
	{
		m_currentToastCount += amount;
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

	bool Currency::RemoveToast(int16_t amount)
	{
		if (m_currentToastCount - amount < 0)
		{
			return false;
		}

		m_currentToastCount -= amount;
		Save();

		//If the currency display exists, update it.
		if (m_currencyDisplay[0] != nullptr)
		{
			RenderCurrencyDisplay();
		}

		return true;
	}

#define TOAST_COUNT_Y_OFFSET 5.f

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

		GUI::Text::TextRenderer::RenderedDigit toastCount = GUI::Text::TextRenderer::RenderNumber(m_currentToastCount, pos - Vec2{ 0.f, TOAST_COUNT_Y_OFFSET }, CRUMB_COUNTER_SCALE, 0.f, 2);

		m_currencyDisplay[0].reset(count[0]);
		m_currencyDisplay[1].reset(count[1]);
		m_currencyDisplay[2].reset(count[2]);

		m_currencyDisplay[4].reset(toastCount[0]);
		m_currencyDisplay[5].reset(toastCount[1]);

		Sprite* sp = Items::ITEMTYPE_GetItemTypeTexture(Items::CRUMBS);
		GUI::StaticGUIElement* crumbImage = new GUI::StaticGUIElement(pos + Vec2{ -count[0]->GetScale().X * 2.f, 0.f }, Vec2{ -count[0]->GetScale().X }, sp->GetSpriteId());
		delete sp;
		Renderer::LoadGUIElement(crumbImage);

		Sprite* toastSp = Items::ITEMTYPE_GetItemTypeTexture(Items::TOAST);
		GUI::StaticGUIElement* toastImage = new GUI::StaticGUIElement(pos - Vec2{ 0.f, TOAST_COUNT_Y_OFFSET } + Vec2{ -toastCount[0]->GetScale().X, 0.f }, Vec2{ -toastCount[0]->GetScale().X }, toastSp->GetSpriteId());
		delete toastSp;
		Renderer::LoadGUIElement(toastImage);


		m_currencyDisplay[3].reset(crumbImage);
		m_currencyDisplay[6].reset(toastImage);
	}

	void Currency::Load()
	{
		int crumbCount = 0, toastCount = 0;
		SaveManager::GetSaveInteger(CURRENCY_CRUMB_COUNT_SAVE_STRING, crumbCount);
		SaveManager::GetSaveInteger(CURRENCY_TOAST_COUNT_SAVE_STRING, toastCount);
		m_currentCrumbCount = crumbCount;
		m_currentToastCount = toastCount;
	}

	void Currency::Save()
	{
		SaveManager::SaveInteger(m_currentCrumbCount, CURRENCY_CRUMB_COUNT_SAVE_STRING);
		SaveManager::SaveInteger(m_currentToastCount, CURRENCY_TOAST_COUNT_SAVE_STRING);
	}
}