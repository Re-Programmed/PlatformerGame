#pragma once
#include <cstdint>
#include <string>
#include "InventoryItem.h"

namespace GAME_NAME::Items
{
	class Inventory
	{
	public:
		Inventory(std::string name, uint8_t size);
		
		struct ReturnItem {
			InventoryItem* ri_Item;
			bool ri_IsNull;
		};
		/// <summary>
		/// Returns a pointer to the item in the given slot.
		/// </summary>
		/// <param name="slot">The slot of the inventory to access the item from.</param>
		/// 
		ReturnItem GetItem(uint8_t slot);

		/// <summary>
		/// Adds an item to the inventory at the last slot.
		/// </summary>
		/// <param name="item">The item to add.</param>
		/// <returns>The slot the item was added to. (-1 if the inventory is full)</returns>
		int AddItem(InventoryItem* item);

		/// <summary>
		/// Sets the item at the given slot to the given item. If item is nullptr, removes the item from the given slot.
		/// </summary>
		/// <param name="slot">The slot to set the item in.</param>
		/// <param name="item">The item to set (or nullptr).</param>
		bool SetItem(uint8_t slot, InventoryItem* item);

		/// <summary>
		/// Returns the amount of a given item type contained in the inventory.
		/// </summary>
		/// <param name="itemType"></param>
		/// <returns></returns>
		unsigned int Contains(ITEM_TYPE itemType);

		/// <summary>
		/// Removes the given type and specified quantity. Will return false if removal failed (but will always remove as many as possible).
		/// </summary>
		/// <param name="type"></param>
		/// <param name="count"></param>
		int Remove(ITEM_TYPE type, unsigned int count);

		inline uint8_t GetSize()
		{
			return m_size;
		}
	protected:
		/// <summary>
		/// The name of the inventory, useful for rendering a graphical display of the inventory.
		/// </summary>
		const std::string m_name;
		/// <summary>
		/// The maximum size of the inventory.
		/// </summary>
		uint8_t m_size;

		std::vector<InventoryItem*> m_items;
	};
}