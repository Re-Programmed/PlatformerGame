#pragma once
#include <cstdint>
#include "../../../Objects/GUI/StaticGUIElement.h"

namespace GAME_NAME::Objects::Player
{
	class Currency
	{
	public:

		static void AddCrumbs(int16_t amount);
		static void AddToast(int16_t amount);
		/// <summary>
		/// Removes crumbs.
		/// </summary>
		/// <param name="amount">How many to remove.</param>
		/// <returns>Returns false if there were not enough crumbs. Will not remove crumbs if not enough are present.</returns>
		static bool RemoveCrumbs(int16_t amount);

		static bool RemoveToast(int16_t amount);

		static void RenderCurrencyDisplay();

		static void Load();

		static void Save();
	private:
		static std::array<std::unique_ptr<GUI::StaticGUIElement>, 7> m_currencyDisplay;

		static uint16_t m_currentCrumbCount, m_currentToastCount;
	};
}