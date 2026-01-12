#include "InventoryTooltip.h"
#include "../../../Input/InputManager.h"
#include <algorithm>
#include "../../../Objects/GUI/Text/TextRenderer.h"


#define TOOLTIP_ANIMATION_SPEED 0.28f

namespace GAME_NAME::Items::Inventories
{
	bool InventoryTooltip::m_hidingTooltip = false;

	StaticGUIElement* InventoryTooltip::m_tooltip;
	std::vector<StaticGUIElement*> InventoryTooltip::m_tooltipComponents;
	std::vector<float> InventoryTooltip::m_tooltipComponentsApproachedScales;

	float InventoryTooltip::m_tooltipHeight = 25.6f;

	/// <summary>
	/// Used to track if the tooltip text should update itself.
	/// </summary>
	uint8_t LastTooltipIndex = 255;
	/// <summary>
	/// Used to track how much the tooltip moved each frame.
	/// </summary>
	std::vector<Vec2> TooltipCompoentDisplacement;
	void InventoryTooltip::UpdateTooltip(uint8_t index, Inventory::ReturnItem item, bool clearTooltip, bool invertTooltipPosition)
	{
		Vec2 MousePosition = InputManager::GetMouseScreenPosition();

		if (clearTooltip)
		{
			m_hidingTooltip = true;

			if (m_tooltip != nullptr)
			{
				if (m_tooltipComponents.size() > 0)
				{
					LastTooltipIndex = 255;

					//Delete existing tooltip data.
					for (StaticGUIElement*& el : m_tooltipComponents)
					{
						Renderer::UnloadGUIElement(el, 2);
						delete el; //no smart pointer ;)
					}
					m_tooltipComponents.clear();
					m_tooltipComponentsApproachedScales.clear();
					TooltipCompoentDisplacement.clear();
				}


				if (m_tooltip->GetScale().Y > 0.05f)
				{
					//Scale down the tool tip by 2% until it has a height less than 0.05.
					DynamicSprite* transparentSprite = new DynamicSprite(m_tooltip->GetSprite()->GetSpriteId());
					float value = std::clamp(m_tooltip->GetScale().Y / (m_tooltipHeight - 3.f), 0.f, 1.f);
					Vec4 textureColor[4] = {
						{ value },
						{ value },
						{ value },
						{ value }
					};
					transparentSprite->UpdateTextureColor(textureColor);
					m_tooltip->SetSprite(transparentSprite);

					m_tooltip->SetScale({ m_tooltip->GetScale().X, m_tooltip->GetScale().Y - (m_tooltip->GetScale().Y * TOOLTIP_ANIMATION_SPEED) });
				}
				else
				{
					//Tooltip is close to small enough, but will never actually reach 0 so set it to 0.
					m_tooltip->SetScale({ m_tooltip->GetScale().X, 0 });
				}
				m_tooltip->SetPosition(MousePosition - Vec2{ 0, m_tooltip->GetScale().Y });

			}

			return;
		}

		if (m_tooltip == nullptr) { return; }

		m_hidingTooltip = false;

		//A different item is now hovered, update the text.
		if (LastTooltipIndex != index || m_tooltipComponents.size() == 0)
		{
			LastTooltipIndex = index;

			//Delete existing tooltip data.
			for (StaticGUIElement*& el : m_tooltipComponents)
			{
				Renderer::UnloadGUIElement(el, 2);
				delete el; //no smart pointer ;)
			}
			m_tooltipComponents.clear();
			m_tooltipComponentsApproachedScales.clear();
			TooltipCompoentDisplacement.clear();

			//Get slot item.
			if (!item.ri_IsNull)
			{
				const ItemData& itemData = ITEMTYPE_GetItemData(item.ri_Item->GetType());

				//Padding + (lines of text needed for name * height of line) + (lines of text needed for desc. * height of desc. text).
				m_tooltipHeight = 5.f + (1 + std::floor(itemData.DisplayName.length() / 10)) * 5.f + (1 + std::count(itemData.Description.begin(), itemData.Description.end(), '\n')) * 3.f;

				//Create item name text.
				Text::TextRenderer::RenderedWord word = Text::TextRenderer::RenderWord(itemData.DisplayName, { m_tooltip->GetPosition().X + 2.f, MousePosition.Y - 2.f }, 5.f, -0.4f, 2);
				m_tooltipComponents.reserve(itemData.DisplayName.length() + itemData.Description.length());

				if (!itemData.Description.empty())
				{
					Text::TextRenderer::ExpectedRenderedWord description = Text::TextRenderer::RenderWordCaseSensitive(itemData.Description, { m_tooltip->GetPosition().X + 2.f, MousePosition.Y - 8.f }, 3.f, -0.4f, 2);


					//All the letters should exist as there was no animation, no need for mutex.
					for (auto letter : description)
					{
						TooltipCompoentDisplacement.emplace_back(letter->letter->GetPosition() - Vec2{ m_tooltip->GetPosition().X, MousePosition.Y - m_tooltipHeight });
						m_tooltipComponents.push_back(letter->letter);
						m_tooltipComponentsApproachedScales.emplace_back(3.f);
					}
				}

				for (StaticGUIElement*& letter : word) { letter->SetScale({ -5.f, 0.f }); TooltipCompoentDisplacement.emplace_back(letter->GetPosition() - Vec2{ m_tooltip->GetPosition().X, MousePosition.Y - m_tooltipHeight }); m_tooltipComponentsApproachedScales.emplace_back(5.f); }

				m_tooltipComponents.insert(m_tooltipComponents.end(), word.begin(), word.end());
			}

		}

		if (m_tooltip->GetScale().Y < m_tooltipHeight)
		{
			DynamicSprite* transparentSprite = new DynamicSprite(m_tooltip->GetSprite()->GetSpriteId());
			float value = std::clamp(m_tooltip->GetScale().Y / (m_tooltipHeight - 3.f), 0.f, 1.f);
			Vec4 textureColor[4] = {
				{ value },
				{ value },
				{ value },
				{ value }
			};
			transparentSprite->UpdateTextureColor(textureColor);
			m_tooltip->SetSprite(transparentSprite);


			//Scale up the tooltip until it reaches the objective m_tooltipHeight.
			m_tooltip->SetScale({ m_tooltip->GetScale().X, m_tooltip->GetScale().Y + (m_tooltipHeight - m_tooltip->GetScale().Y) * TOOLTIP_ANIMATION_SPEED });
		}
		else {
			m_tooltip->SetScale({ m_tooltip->GetScale().X, m_tooltipHeight });
		}

		m_tooltip->SetPosition(MousePosition - Vec2{ 0, m_tooltip->GetScale().Y });
		if (invertTooltipPosition)
		{
			m_tooltip->SetPosition(m_tooltip->GetPosition() - Vec2{ m_tooltip->GetScale().X, 0.f });
		}

		float value = std::clamp(m_tooltip->GetScale().Y / (m_tooltipHeight - 3.f), 0.f, 1.f);
		Vec4 textureColor[4] = {
			{ value },
			{ value },
			{ value },
			{ value }
		};

		int i = 0;
		//The tool tip moved, update all the components to follow its path.
		for (StaticGUIElement*& el : m_tooltipComponents)
		{
			DynamicSprite* transparentSprite = new DynamicSprite(el->GetSprite()->GetSpriteId());
			transparentSprite->UpdateTextureColor(textureColor);
			el->SetSprite(transparentSprite);

			el->SetPosition(m_tooltip->GetPosition() + TooltipCompoentDisplacement[i]);
			el->SetScale({ el->GetScale().X > 0.f ? el->GetScale().X : -el->GetScale().X, -std::lerp(-el->GetScale().Y, m_tooltipComponentsApproachedScales[i], 0.08f) });
			i++;
		}
	}

	void InventoryTooltip::RemoveTooltip()
	{
		if (m_tooltip == nullptr) { return; }

		//Remove tooltip.
		Renderer::UnloadGUIElement(m_tooltip, 2);
		delete m_tooltip;
		m_tooltip = nullptr;

		for (auto& component : m_tooltipComponents)
		{
			Renderer::UnloadGUIElement(component, 2);
			delete component;
		}
		m_tooltipComponents.clear();
	}

	void InventoryTooltip::CreateTooltip()
	{
		if (m_tooltip == nullptr)
		{
			m_tooltip = new StaticGUIElement({ 0, 0 }, { 62, 0 }, Renderer::GetSprite(SpriteBase(73))->GetSpriteId());
			Renderer::LoadGUIElement(m_tooltip, 2);
		}
	}
}