#include "InventoryTooltip.h"
#include "../../../Input/InputManager.h"
#include <algorithm>

#include "../../Cutscenes/DialogueManager.h"

#include "../../Items/Types/Firearm.h"

#define TOOLTIP_ANIMATION_SPEED 0.28f

namespace GAME_NAME::Items::Inventories
{
	bool InventoryTooltip::m_hidingTooltip = false;

	std::vector<InventoryTooltip::TooltipElement> InventoryTooltip::m_tooltip;
	std::vector<StaticGUIElement*> InventoryTooltip::m_tooltipComponents;
	std::vector<Vec2> InventoryTooltip::m_tooltipComponentsApproachedScales;

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

			if (m_tooltip.size() != 0)
			{
				if (m_tooltipComponents.size() > 0)
				{
					LastTooltipIndex = 255;

					//Delete existing tooltip data.
					for (StaticGUIElement*& el : m_tooltipComponents)
					{
						Renderer::UnloadGUIElement(el, 2);
						delete el;
					}
					m_tooltipComponents.clear();
					m_tooltipComponentsApproachedScales.clear();
					TooltipCompoentDisplacement.clear();
				}

				for (TooltipElement& tooltip : m_tooltip)
				{

					if (tooltip.Element->GetScale().Y > 0.05f)
					{
						//Scale down the tool tip by 2% until it has a height less than 0.05.
						DynamicSprite* transparentSprite = new DynamicSprite(tooltip.Element->GetSprite()->GetSpriteId());
						float value = std::clamp(tooltip.Element->GetScale().Y / (m_tooltipHeight - 3.f), 0.f, 1.f);
						Vec4 textureColor[4] = {
							{ value },
							{ value },
							{ value },
							{ value }
						};
						transparentSprite->UpdateTextureColor(textureColor);
						tooltip.Element->SetSprite(transparentSprite);

						tooltip.Element->SetScale({ tooltip.Element->GetScale().X, tooltip.Element->GetScale().Y - (tooltip.Element->GetScale().Y * TOOLTIP_ANIMATION_SPEED) });
					}
					else
					{
						//Tooltip is close to small enough, but will never actually reach 0 so set it to 0.
						tooltip.Element->SetScale({ tooltip.Element->GetScale().X, 0 });
					}

					//tooltip.Element->SetPosition(MousePosition - Vec2{ 0, tooltip.Element->GetScale().Y } + tooltip.Offset - Vec2{ 0.f, 32.f });
				}


			}

			return;
		}

		if (m_tooltip.size() == 0) { return; }

		m_hidingTooltip = false;

		//A different item is now hovered, update the text.
		if (LastTooltipIndex != index || m_tooltipComponents.size() == 0)
		{
			LastTooltipIndex = index;

			//Delete existing tooltip data.
			for (StaticGUIElement*& el : m_tooltipComponents)
			{
				Renderer::UnloadGUIElement(el, 2);
				delete el;
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
				Text::TextRenderer::RenderedWord word = Text::TextRenderer::RenderWord(itemData.DisplayName, { m_tooltip[0].Element->GetPosition().X + 2.f, MousePosition.Y - 2.f}, 5.f, -0.4f, 2);
				m_tooltipComponents.reserve(itemData.DisplayName.length() + itemData.Description.length());

				if (!itemData.Description.empty())
				{
					Text::TextRenderer::ExpectedRenderedWord description = Text::TextRenderer::RenderWordCaseSensitive(itemData.Description, { m_tooltip[0].Element->GetPosition().X + 2.f, MousePosition.Y - 8.f }, 3.f, -0.4f, 2);


					//All the letters should exist as there was no animation, no need for mutex.
					for (auto letter : description)
					{
						TooltipCompoentDisplacement.emplace_back(letter->letter->GetPosition() - Vec2{ m_tooltip[0].Element->GetPosition().X, MousePosition.Y - m_tooltipHeight });
						m_tooltipComponents.push_back(letter->letter);
						m_tooltipComponentsApproachedScales.emplace_back(3.f);
					}
				}

				for (StaticGUIElement*& letter : word) { letter->SetScale({ -5.f, 0.f }); TooltipCompoentDisplacement.emplace_back(letter->GetPosition() - Vec2{ m_tooltip[0].Element->GetPosition().X, MousePosition.Y - m_tooltipHeight }); m_tooltipComponentsApproachedScales.emplace_back(5.f); }

				m_tooltipComponents.insert(m_tooltipComponents.end(), word.begin(), word.end());

				while (itemData.Attributes.size() < m_tooltip.size() - 1)
				{
					StaticGUIElement* element = m_tooltip[m_tooltip.size() - 1].Element;
					Renderer::UnloadGUIElement(element, 2);
					delete element;

					m_tooltip.pop_back();
				}

				//Display item's attributes in new boxes.
				if (itemData.Attributes.size() > 0)
				{
					size_t i = 1;
					for (auto& [action, data] : itemData.Attributes)
					{
						//USE_SOUND shouldn't give a tooltip.
						if (action == TOOL_ACTION::USE_SOUND) { continue; }

						//Create a new box to show the new information.
						while(m_tooltip.size() <= i)
						{
							float offset = m_tooltip[m_tooltip.size() - 1].Offset.Y - m_tooltipHeight;
							StaticGUIElement* element = new StaticGUIElement({ 0, offset }, { 40, 0 }, Renderer::GetSprite(SpriteBase(73))->GetSpriteId());
							m_tooltip.emplace_back(element, Vec2{ 0.f, offset });
							Renderer::LoadGUIElement(element, 2);
						}

						Vec2 pos(m_tooltip[i].Element->GetPosition().X + 2.f, MousePosition.Y - 8.f + m_tooltip[i].Offset.Y);
						Text::TextRenderer::ExpectedRenderedWord attributeDescription = formatToolAttributeHeading(pos, action, data);

						Text::TextRenderer::RenderedDigit attributeValues = formatToolAttributeValue(pos - Vec2{ 0.f, 4.f }, action, data, item.ri_Item);

						for (auto& letter : attributeDescription)
						{
							TooltipCompoentDisplacement.emplace_back(letter->letter->GetPosition() - Vec2{ m_tooltip[i].Element->GetPosition().X, MousePosition.Y - m_tooltipHeight - 5.f });
							m_tooltipComponents.push_back(letter->letter);
							m_tooltipComponentsApproachedScales.emplace_back(4.f);
						}

						for (auto& digit : attributeValues)
						{
							TooltipCompoentDisplacement.emplace_back(digit->GetPosition() - Vec2{ m_tooltip[i].Element->GetPosition().X, MousePosition.Y - m_tooltipHeight - 4.f });
							m_tooltipComponents.push_back(digit);
							m_tooltipComponentsApproachedScales.emplace_back(digit->GetScale());
						}

						i++;
					}
				}
			}

		}

		for (InventoryTooltip::TooltipElement& tooltip : m_tooltip)
		{
			if (tooltip.Element->GetScale().Y < m_tooltipHeight)
			{
				DynamicSprite* transparentSprite = new DynamicSprite(tooltip.Element->GetSprite()->GetSpriteId());
				float value = std::clamp(tooltip.Element->GetScale().Y / (m_tooltipHeight - 3.f), 0.f, 1.f);
				Vec4 textureColor[4] = {
					{ value },
					{ value },
					{ value },
					{ value }
				};
				transparentSprite->UpdateTextureColor(textureColor);
				tooltip.Element->SetSprite(transparentSprite);


				//Scale up the tooltip until it reaches the objective m_tooltipHeight.
				tooltip.Element->SetScale({ tooltip.Element->GetScale().X, tooltip.Element->GetScale().Y + (m_tooltipHeight - tooltip.Element->GetScale().Y) * TOOLTIP_ANIMATION_SPEED });
			}
			else {
				tooltip.Element->SetScale({ tooltip.Element->GetScale().X, m_tooltipHeight });
			}

			tooltip.Element->SetPosition(MousePosition - Vec2{ 0, tooltip.Element->GetScale().Y } + tooltip.Offset);
			if (invertTooltipPosition)
			{
				tooltip.Element->SetPosition(tooltip.Element->GetPosition() - Vec2{ tooltip.Element->GetScale().X, 0.f } + tooltip.Offset);
			}

		}

		float value = std::clamp(m_tooltip[0].Element->GetScale().Y / (m_tooltipHeight - 3.f), 0.f, 1.f);
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

			el->SetPosition(m_tooltip[0].Element->GetPosition() + TooltipCompoentDisplacement[i]);
			el->SetScale({ el->GetScale().X > 0.f ? el->GetScale().X : -el->GetScale().X, -std::lerp(-el->GetScale().Y, m_tooltipComponentsApproachedScales[i].Y, 0.08f) });
			i++;
		}
	}

	void InventoryTooltip::RemoveTooltip()
	{
		if (m_tooltip.size() == 0) { return; }

		//Remove tooltip.
		for (TooltipElement& tooltip : m_tooltip)
		{
			Renderer::UnloadGUIElement(tooltip.Element, 2);
			delete tooltip.Element;
		}

		m_tooltip.clear();

		for (auto& component : m_tooltipComponents)
		{
			Renderer::UnloadGUIElement(component, 2);
			delete component;
		}
		m_tooltipComponents.clear();
	}

	void InventoryTooltip::CreateTooltip()
	{
		if (m_tooltip.size() == 0)
		{
			m_tooltip.emplace_back(new StaticGUIElement({ 0, 0 }, { 62, 0 }, Renderer::GetSprite(SpriteBase(73))->GetSpriteId()), Vec2{ 0.f, 0.f });
			
			for (TooltipElement& element : m_tooltip)
			{
				Renderer::LoadGUIElement(element.Element, 2);
			}
		}
	}

	/// <summary>
	/// Creates the display for the title of a tooltip box based on what an item's abilities are.
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="action"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	Text::TextRenderer::ExpectedRenderedWord InventoryTooltip::formatToolAttributeHeading(Vec2 pos, const TOOL_ACTION& action, const std::string& data)
	{
		Text::TextRenderer::ExpectedRenderedWord word;

		switch (action)
		{
		case TOOL_ACTION::FURNITURE:
			//This is outdoor furniture.
			if (data.ends_with(",1"))
			{
				word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_FurnitureOutdoor"), pos, 4.f, -0.4f, 2);
			}
			else {
				word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_FurnitureIndoor"), pos, 4.f, -0.4f, 2);
			}
			break;
		case TOOL_ACTION::MINE:
			word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_Mine"), pos, 4.f, -0.4f, 2);
			break;
		case TOOL_ACTION::PLACEABLE:
			word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_Placeable"), pos, 4.f, -0.4f, 2);
			break;

		case TOOL_ACTION::SHIELD:
			word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_Shield"), pos, 4.f, -0.4f, 2);
			break;

		case TOOL_ACTION::EQUIPMENT:
			//Equipment can have different attributes.
			if (data.starts_with("Health"))
			{
				word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_EquipmentHealth"), pos, 4.f, -0.4f, 2);
			}
			break;
		case TOOL_ACTION::CHOP:
			word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_Chop"), pos, 4.f, -0.4f, 2);
			break;
		case TOOL_ACTION::VALUE:
			word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_Value"), pos, 4.f, -0.4f, 2);
			break;
		case TOOL_ACTION::FOOD:
			word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_Food"), pos, 4.f, -0.4f, 2);
			break;
		case TOOL_ACTION::RANGED_WEAPON:
		case TOOL_ACTION::WEAPON:
			word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_Damage"), pos, 4.f, -0.4f, 2);
			break;
		case TOOL_ACTION::FIREARM:
			word = Text::TextRenderer::RenderWordCaseSensitive(Cutscenes::DialogueManager::INSTANCE->GetPhrase("InventoryTooltip_Firearm"), pos, 4.f, -0.4f, 2);
			break;
		}

		return word;
	}

	Text::TextRenderer::RenderedDigit InventoryTooltip::formatToolAttributeValue(Vec2 pos, const TOOL_ACTION& action, const std::string& data, InventoryItem* item)
	{
		if (action & TOOL_ACTION::EQUIPMENT)
		{
			//Equipment can have different attributes.
			if (data.starts_with("Health"))
			{
				return Text::TextRenderer::RenderNumber(std::stoi(data.substr(data.find_first_of(":") + 1)), pos, 0.175f, 0.f, 1, TEXT_RENDERER_ZERO_DIGIT_SPRITE_ID, 2);
			}
		}

		if (action & TOOL_ACTION::VALUE)
		{
			return Text::TextRenderer::RenderNumber(std::stoi(data), pos, 0.175f, 0.f, 1, TEXT_RENDERER_ZERO_DIGIT_SPRITE_ID, 2);
		}

		if (action & TOOL_ACTION::FOOD)
		{
			return Text::TextRenderer::RenderNumber(std::stoi(data), pos, 0.175f, 0.f, 1, TEXT_RENDERER_ZERO_DIGIT_SPRITE_ID, 2);
		}

		if (action & TOOL_ACTION::WEAPON)
		{
			int damage = std::stoi(data.substr(0, data.find_first_of(',')));
			return Text::TextRenderer::RenderNumber(damage, pos, 0.175f, 0.f, 1, TEXT_RENDERER_ZERO_DIGIT_SPRITE_ID, 2);
		}

		if (action & TOOL_ACTION::FIREARM)
		{
			Firearm::FirearmStats stats = Firearm::GetStats(data);

			Firearm* f = dynamic_cast<Firearm*>(item);

			if (f)
			{
				return Text::TextRenderer::RenderNumber(f->GetAmmo(), pos, 0.175f, 0.f, 1, TEXT_RENDERER_ZERO_DIGIT_SPRITE_ID, 2);
			}
		}

		return Text::TextRenderer::RenderedDigit();
	}
}