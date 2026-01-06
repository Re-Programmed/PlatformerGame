#include "./InventoryItem.h"

#include "./Types/Tool.h"
#include "./Types/Weapon.h"
#include "./Types/Food.h"
#include "./Types/Placeable.h"
#include "./Types/Blueprint.h"

namespace GAME_NAME
{
	namespace Items
	{
		InventoryItem::InventoryItem(ITEM_TYPE type)
			: m_itemType(type)
		{

		}

		/*InventoryItem::InventoryItem(const InventoryItem& item)
			: m_itemType(item.m_itemType)
		{
			
		}*/

		/*InventoryItem::InventoryItem(InventoryItem&& item) noexcept
		{
			m_itemType = item.m_itemType;
		}*/


		MiscState::SaveParam InventoryItem::Encode()
		{
			MiscState::SaveParam param;
			param += getPrefix();
			return param.append(std::to_string(m_itemType));
		}

		//Expects a string that is purely a number representing the type.
		void InventoryItem::Decode(const MiscState::SaveParam param)
		{
			//Get the item type, ignore the first character that denotes item class type.
			m_itemType = (Items::ITEM_TYPE)std::stoi(param.substr(1));
		}



		InventoryItem* InventoryItem::DecodeItemString(std::string data)
		{
			//First char indicates the type of the item, and what class should be used to represent it.
			const char& firstChar = data.at(0);

			InventoryItem* item = new InventoryItem();

			switch (firstChar)
			{
			case ITEM_PREFIX_ITEM:
				//Decode the item.
				item->Decode(data);

				//Item should be null.
				if (item->GetType() == NULL_ITEM)
				{
					delete item;
					item = nullptr;
				}

				break;
			case ITEM_PREFIX_TOOL:
			{
				//Change item to point to a new tool.
				delete item;
				Tool* t = new Tool();
				t->Decode(data);
				item = t;
				break;
			}

			case ITEM_PREFIX_WEAPON:
			{
				//Change item to point to a new weapon.
				delete item;
				Weapon* w = new Weapon();
				w->Decode(data);
				item = w;
				break;
			}

			case ITEM_PREFIX_FOOD:
			{
				delete item;
				Food* f = new Food();
				f->Decode(data);
				item = f;
				break;
			}

			case ITEM_PREFIX_PLACEABLE:
			{
				delete item;
				Placeable* p = new Placeable();
				p->Decode(data);
				item = p;
				break;
			}

			case ITEM_PREFIX_BLUEPRINT:
			{
				delete item;
				Blueprint* bp = new Blueprint();
				bp->Decode(data);
				item = bp;
				break;
			}
			}

			return item;
		}

		std::unique_ptr<InventoryItem> InventoryItem::GetUnique()
		{
			return std::unique_ptr<InventoryItem>(this);
		}
	}
}