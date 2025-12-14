#include "GUIItemSelectionBox.h"

#include "../../../Objects/GUI/GUIManager.h"
#include <algorithm>
#include "../../../Objects/GUI/Text/TextRenderer.h"

#define GUI_ITEM_SELECTION_BOX_OPTION_WIDTH 44.f
#define GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT 10.f

#define GUI_ITEM_SELECTION_BOX_OPTION_BG_SPRITE SpriteBase(255)

#define GUI_ITEM_SELECTION_BOX_PADDING 3.f

#define GUI_ITEM_SELECTION_SCROLL_INDICATOR_SPRITE SpriteBase(253)
#define GUI_ITEM_SELECTION_SCROLL_INDICATOR_HEIGHT 5.f

#define GUI_ITEM_SELECTION_COVER_SPRITE SpriteBase(254)
namespace GAME_NAME::Objects::GUI
{
	GUIItemSelectionBox::GUIItemSelectionBox(Vec2 position, float cutoffHeight, ItemSelection items[], size_t itemCount, std::function<void(Items::ITEM_TYPE, int)>* selectedCallback)
		: GUIScrollArea(position, Vec2{ GUI_ITEM_SELECTION_BOX_OPTION_WIDTH, cutoffHeight } + Vec2{ GUI_ITEM_SELECTION_BOX_PADDING * 2.f }, 0 /*No Sprite*/, -(itemCount * GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT - cutoffHeight), 0.0)
	{
		m_selectedCallback.reset(selectedCallback);

		m_itemContents.reserve(itemCount);

		for (uint8_t i = 0; i < itemCount; i++)
		{
			AddItemSelection(items[i]);
		}
	}

	GUIItemSelectionBox::~GUIItemSelectionBox()
	{
		//All the item selectioon stuff is managed by this class, so it needs to handle the deleting. (or use unique_ptr...)
		for (StaticGUIElement* element : m_contents)
		{
			Renderer::UnloadGUIElement(element);

			GUIButton* button = dynamic_cast<GUIButton*>(element);
			if (button)
			{
				GUIManager::UnregisterButton(button);
			}

			delete element; 
		}

		GUIScrollArea::~GUIScrollArea();
	}

	void GUIItemSelectionBox::Render(float zoom)
	{
		//Disable scroll contents if they are not within the scale of the scroll area.
		for (StaticGUIElement* element : m_contents)
		{
			element->SetActive(element->GetPosition().Y < this->m_position.Y + this->m_scale.Y && element->GetPosition().Y + element->GetScale().Y > this->m_position.Y);
		}

		Sprite* bottomCover = Renderer::GetSprite(GUI_ITEM_SELECTION_COVER_SPRITE);
		bottomCover->Render(Vec2::Zero, m_position - Vec2{ 0.f, GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT }, Vec2{ m_scale.X, GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT }, 0.f);
		delete bottomCover;

		Sprite* topCover = Renderer::GetSprite(GUI_ITEM_SELECTION_COVER_SPRITE);
		topCover->Render(Vec2::Zero, m_position + Vec2{ 0.f, m_scale.Y }, Vec2{ m_scale.X, GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT }, 0.f);
		delete topCover;

		if (m_currentY > m_minY + 1.f)
		{
			Sprite* scrollDownIndicator = Renderer::GetSprite(GUI_ITEM_SELECTION_SCROLL_INDICATOR_SPRITE);
			scrollDownIndicator->Render(Vec2::Zero, m_position - Vec2{ 0.f, GUI_ITEM_SELECTION_SCROLL_INDICATOR_HEIGHT }, Vec2{ m_scale.X, GUI_ITEM_SELECTION_SCROLL_INDICATOR_HEIGHT }, 0.f);
			delete scrollDownIndicator;
		}

		if (m_currentY < m_maxY - 1.f)
		{
			Sprite* scrollUpIndicator = Renderer::GetSprite(GUI_ITEM_SELECTION_SCROLL_INDICATOR_SPRITE);
			scrollUpIndicator->Render(Vec2::Zero, m_position + Vec2{ 0.f, m_scale.Y + GUI_ITEM_SELECTION_SCROLL_INDICATOR_HEIGHT }, Vec2{ m_scale.X, -GUI_ITEM_SELECTION_SCROLL_INDICATOR_HEIGHT }, 0.f);
			delete scrollUpIndicator;
		}

		GUIScrollArea::Render(zoom);
	}

	void GUIItemSelectionBox::AddItemSelection(ItemSelection item)
	{
		Vec2 position = m_position + GUI_ITEM_SELECTION_BOX_PADDING + Vec2{ 0.f, GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT * m_itemContents.size() };

		Sprite* sp = Renderer::GetSprite(GUI_ITEM_SELECTION_BOX_OPTION_BG_SPRITE);
		GUIButton* optionBackground = new GUIButton(position, Vec2{ GUI_ITEM_SELECTION_BOX_OPTION_WIDTH, GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT }, sp->GetSpriteId(), new std::function<void(int)>(GUIItemSelectionBox::OptionClicked),
			Vec4{ 1.f, 1.f, 1.f, 1.f },
			Vec4{ 0.5f, 1.f, 0.5f, 1.f });
		delete sp;

		Sprite* itemSprite = Items::ITEMTYPE_GetItemTypeTexture(item.Type);
		StaticGUIElement* itemDisplay = new StaticGUIElement(position + Vec2{ GUI_ITEM_SELECTION_BOX_PADDING / 2.f }, Vec2{ GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT - GUI_ITEM_SELECTION_BOX_PADDING }, itemSprite->GetSpriteId());
		delete itemSprite;

		Renderer::LoadGUIElement(optionBackground);
		Renderer::LoadGUIElement(itemDisplay);

		Vec2 digitPos = position + Vec2{ 2 * itemDisplay->GetScale().X + GUI_ITEM_SELECTION_BOX_PADDING / 2.f, 2.f };
		Text::TextRenderer::RenderedDigit countDisplay = Text::TextRenderer::RenderNumber(item.Count, digitPos, (GUI_ITEM_SELECTION_BOX_OPTION_HEIGHT - GUI_ITEM_SELECTION_BOX_PADDING) / 30.f, 0.f, 1);

		this->AddElement(optionBackground);
		this->AddElement(itemDisplay);

		for (GUI::StaticGUIElement* digit : countDisplay)
		{
			this->AddElement(digit);
		}

		item.ButtonID = GUIManager::RegisterButton(optionBackground);
		m_itemContents.push_back(item);
	}

	void GUIItemSelectionBox::OptionClicked(int buttonID)
	{
		for (GUIScrollArea* area : m_areas)
		{
			//Determine the selected item from the selected area and call its respective callback.

			GUIItemSelectionBox* itemArea = dynamic_cast<GUIItemSelectionBox*>(area);
			if (itemArea)
			{
				bool isValid = false;
				GUIItemSelectionBox::ItemSelection itemSelected = itemArea->GetItemSelected(buttonID, isValid);

				if (isValid)
				{
					itemArea->selectedCallback(itemSelected.Type, itemSelected.Count);
					return;	//We found it, so return.
				}
			}
		}
	}

	//Returns the items with matching buttonIDs.
	//isValid will indicate if the specified value exists.
	GUIItemSelectionBox::ItemSelection GUIItemSelectionBox::GetItemSelected(int buttonID, bool& isValid)
	{
		std::vector<GUIItemSelectionBox::ItemSelection> ret;

		std::copy_if(m_itemContents.begin(), m_itemContents.end(), std::back_inserter(ret), [buttonID](GUIItemSelectionBox::ItemSelection selection) {
			return selection.ButtonID == buttonID;
		});

		if (ret.size() == 0)
		{
			isValid = false;
			return GUIItemSelectionBox::ItemSelection();
		}

		isValid = true;
		return ret[0];
	}



}
