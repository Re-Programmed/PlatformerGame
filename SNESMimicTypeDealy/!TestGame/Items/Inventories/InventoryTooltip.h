#pragma once
#include "../../../Objects/GUI/StaticGUIElement.h"
#include "./InventoryContainer.h"
#include "../../../Objects/GUI/Text/TextRenderer.h"

namespace GAME_NAME::Items::Inventories 
{
	using namespace Objects::GUI;

	class InventoryTooltip
	{
	public:
		static void UpdateTooltip(uint8_t index, Inventory::ReturnItem item, bool clearTooltip = false, bool invertTooltipPosition = false);

		static void RemoveTooltip();

		static void CreateTooltip();

	private:
		static Text::TextRenderer::ExpectedRenderedWord formatToolAttributeHeading(Vec2 pos, const TOOL_ACTION& action, const std::string& data);
		static Text::TextRenderer::RenderedDigit formatToolAttributeValue(Vec2 pos, const TOOL_ACTION& action, const std::string& data);

		static bool m_hidingTooltip;

		struct TooltipElement
		{
			StaticGUIElement* const Element;
			const Vec2 Offset;
		};

		static std::vector<TooltipElement> m_tooltip;
		/// <summary>
		/// The objects that make up the parts of the tooltip.
		/// </summary>
		static std::vector<StaticGUIElement*> m_tooltipComponents;
		static std::vector<Vec2> m_tooltipComponentsApproachedScales;

		/// <summary>
		/// How tall the curent tooltip is. This will change based on how much text must display.
		/// </summary>
		static float m_tooltipHeight;

	};
}