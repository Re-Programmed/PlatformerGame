#pragma once
#include "./ItemType.h"
#include "../../Objects/MiscState.h"

#define ITEM_PREFIX_ITEM 'i'

namespace GAME_NAME
{
	namespace Items
	{
		/// <summary>
		/// An inventory item represents an item's data.
		/// </summary>
		class InventoryItem
			: public MiscState
		{
		public:
			/// <summary>
			/// Create an inventory item with type.
			/// </summary>
			/// <param name="type">The type of item to create.</param>
			InventoryItem(ITEM_TYPE type);

			InventoryItem() SERIALIZED = default;

			//InventoryItem(const InventoryItem& item);
			//InventoryItem(InventoryItem&& item) noexcept;

			/// <summary>
			/// Returns the type of item.
			/// </summary>
			/// <returns></returns>
			inline ITEM_TYPE GetType()
			{
				return m_itemType;
			}

			SaveParam Encode() override;
			void Decode(const SaveParam params) override;

			static InventoryItem* DecodeItemString(std::string data);

			virtual std::unique_ptr<InventoryItem> GetUnique();
		protected:
			/// <summary>
			/// The type of the item represented by this InventoryItem.
			/// </summary>
			ITEM_TYPE m_itemType SERIALIZED;

			/// <summary>
			/// Returns the prefix used to save this type of item.
			/// </summary>
			/// <returns></returns>
			inline virtual constexpr const char getPrefix() { return ITEM_PREFIX_ITEM; }
		};
	}
}