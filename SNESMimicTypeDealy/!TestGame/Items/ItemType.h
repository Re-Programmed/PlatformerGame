#pragma once
#include "../../Rendering/Sprite.h"
#include "../../Rendering/Renderers/Renderer.h"

#include <unordered_map>

#include "../../Audio/SoundEvents.h"

#define ITEM_PREFIX_ITEM 'i'
#define ITEM_PREFIX_TOOL 't'
#define ITEM_PREFIX_WEAPON 'w'

namespace GAME_NAME
{
	namespace Items
	{
#define ITEM_COUNT 31
		/// <summary>
		/// An enum for the types of items in the game.
		/// </summary>
		enum ITEM_TYPE
		{
			LOG = 0,
			LEAVES = 1,
			STONE = 2,
			SCRAP_AXE = 3,
			NULL_ITEM = 4, //NULL ITEM IS 4 (used for saving data about items that are not there)
			WOODEN_SHOES = 5,
			SHARP_STICK = 6,
			TOAST = 7,
			APPLE = 8,
			WOODEN_PLANK = 9,
			CRUMBS = 10,
			BLUEPRINT = 11,
			SMALL_WOODEN_CHAIR = 12,
			FRIDGE = 13,
			WOODEN_CRATE = 14,
			WORKBENCH = 15,
			BUTTER_KNIFE = 16,
			IRON_SWORD = 17,
			WOODEN_CLUB = 18,
			SPEAR = 19,
			WOODEN_PLATING = 20,
			LEAF_HELMET = 21,
			TWIG = 22,
			CLOTH = 23,
			WET_WASHCLOTH = 24,
			SMALL_STONE = 25,
			LEVEL_SAVE_POINT = 26,
			SCREW = 27,
			BOMB = 28,
			GRENADE = 29,
			C4 = 30
		};

		/// <summary>
		/// Different actions that tools can perform. Can be checked for when determining wether the player can perform some action based on what tool they are holding.
		/// </summary>
		enum TOOL_ACTION
		{
			CHOP =			0b0000000001, //Can cut down trees.
			EQUIPMENT =		0b0000000010, //Can be equipped to equipment slots.
			MINE =			0b0000000100, //Can break BreakableBlocks. (provide a strength attribute)
			WEAPON =		0b0000001000, //Can be used to attack. (provide a damage, power consumption, reload attribute, area of effect, and sound [damage,powerconsume,reloadseconds,AOE(integer),sound=Punch])
			FOOD =			0b0000010000, //Can be eaten.
			PLACEABLE =		0b0000100000, //Can be placed.
			FURNITURE =		0b0001000000, //Furniture. (xScale, yScale, inventorySize[optional])
			VALUE =			0b0010000000, //Give an item a specific price value. (crumbValue)
			RANGED_WEAPON = 0b0100000000, //Shoots projectiles [damage,powerconsume,reloadseconds,range,projectile_type]
			USE_SOUND =		0b1000000000, //Plays a different sound when used. [soundEventNumber]
		};

		/// <summary>
		/// ItemData is the data for different item types that remains constant.
		/// (ex. Every log will always be named "Log," so the name is stored in the data for log.
		/// </summary>
		struct ItemData
		{
			/// <summary>
			/// The name of the item.
			/// </summary>
			const std::string DisplayName;
			/// <summary>
			/// The texture for the inventory display of the item.
			/// </summary>
			const int16_t Texture; //WHY was this uint :(:(:( this caused so umuch pain.
			/// <summary>
			/// The texture for holding the item. 
			/// </summary>
			const int16_t HeldTexture;
			/// <summary>
			/// A flag list of things the item can do.
			/// </summary>
			const uint16_t Actions = 0;
			/// <summary>
			/// Stores data about the items actions like its strength.
			/// </summary>
			const std::unordered_map<TOOL_ACTION, std::string> Attributes;
			/// <summary>
			/// Whatever you want.
			/// </summary>
			const std::string Description;
		};

#define NO_HELD_TEXTURE GLOBAL_SPRITE_BASE
#define FOLLOW_HAND_TEXTURE (GLOBAL_SPRITE_BASE - 1)

		/// <summary>
		/// A list of the item data for each item type.
		/// </summary>
		const ItemData ITEM_DATA[ITEM_COUNT]
		{
			{ "Log", SpriteBase(248), NO_HELD_TEXTURE, 0, {}, "Log of wood."},	//0
			{ "Leaves", SpriteBase(246), NO_HELD_TEXTURE, 0, {}, ""},		//1
			{ "Stone", 55, NO_HELD_TEXTURE, 0, {}, "" },		//2
			{ "(Temp) Axe", SpriteBase(65), SpriteBase(65), WEAPON | CHOP, {{TOOL_ACTION::CHOP , "1"}, {TOOL_ACTION::WEAPON, "4,0,0.9,20"}}, ""},	//3
			{ "Unknown", 0, NO_HELD_TEXTURE, 0, {}, "" },																	//4
			{ "Wooden Shoes", 18, NO_HELD_TEXTURE, EQUIPMENT, { { TOOL_ACTION::EQUIPMENT, "Health:10" } }, "" },			//5
			{ "Sharp Stick", SpriteBase(118), SpriteBase(117), WEAPON, {{TOOL_ACTION::WEAPON, "3,0,0.2,26"}}, ""},			//6
			{ "Toast", SpriteBase(132), SpriteBase(233), 0, {}, "Probably made in a toaster." },							//7
			{ "Apple", SpriteBase(247), NO_HELD_TEXTURE, FOOD, { { TOOL_ACTION::FOOD, "20" } }, "Apple."},					//8
			{ "Wooden Plank", SpriteBase(249), NO_HELD_TEXTURE, PLACEABLE, { { TOOL_ACTION::PLACEABLE, "32,6" }}, "Can be placed."},			//9
			{ "Crumb", SpriteBase(257), NO_HELD_TEXTURE, 0, {}, "Looks like money..."},					//10
			{ "Blueprint", SpriteBase(263), NO_HELD_TEXTURE, 0, {}, "Unlocks a recipe."},					//11

			{ "Small Wooden Chair", 2, NO_HELD_TEXTURE, FURNITURE | VALUE, { { TOOL_ACTION::FURNITURE, "15,22.5" }, { TOOL_ACTION::VALUE, "10" } }, "Might give splinters."},					//12
			{ "Fridge", 14, NO_HELD_TEXTURE, FURNITURE | VALUE, { { TOOL_ACTION::FURNITURE, "19.5,33,6" }, { TOOL_ACTION::VALUE, "42" } }, "Holds six items."},					//13
			{ "Wooden Crate", 19, NO_HELD_TEXTURE, FURNITURE | VALUE, { { TOOL_ACTION::FURNITURE, "17.25,16.5,10" }, { TOOL_ACTION::VALUE, "16" } }, "Holds tem items."},					//14
			{ "Workbench", 22, NO_HELD_TEXTURE, FURNITURE | VALUE, { { TOOL_ACTION::FURNITURE, "26,29" }, { TOOL_ACTION::VALUE, "45" } }, "Make stuff with me!"},					//15

			{ "Butter Knife", SpriteBase(292), SpriteBase(292), WEAPON | VALUE, { { TOOL_ACTION::WEAPON, std::string("4,0,0.3,17,").append(std::to_string(static_cast<int>(Audio::SoundEvents::Event::HIT_SLICED))) }, {TOOL_ACTION::VALUE, "12"}}, "Still pretty sharp."},					//16
			{ "Iron Sword", SpriteBase(287), SpriteBase(287), WEAPON | VALUE, { { TOOL_ACTION::WEAPON, std::string("8,0,0.9,22,").append(std::to_string(static_cast<int>(Audio::SoundEvents::Event::HIT_SLICED))) }, { TOOL_ACTION::VALUE, "60" }}, "8 Damage."},					//17
			{ "Wooden Club", SpriteBase(297), FOLLOW_HAND_TEXTURE, WEAPON | VALUE, { { TOOL_ACTION::WEAPON, "6,0,1.66,26" }, { TOOL_ACTION::VALUE, "30" }}, "Slow but hurts."},					//18
			{ "Spear", SpriteBase(298), FOLLOW_HAND_TEXTURE, WEAPON | VALUE, { { TOOL_ACTION::WEAPON, std::string("5,0,1.2,45,").append(std::to_string(static_cast<int>(Audio::SoundEvents::Event::HIT_SLICED))) }, { TOOL_ACTION::VALUE, "30" }}, "Reach really far."},					//19

			{ "Wooden Plating", SpriteBase(374), NO_HELD_TEXTURE, EQUIPMENT | VALUE, { { TOOL_ACTION::EQUIPMENT, "Health:15" }, { TOOL_ACTION::VALUE, "20" }}, "15 health."},					//20
			{ "Leaf Helmet", SpriteBase(375), NO_HELD_TEXTURE, EQUIPMENT | VALUE, { { TOOL_ACTION::EQUIPMENT, "Health:5" }, { TOOL_ACTION::VALUE, "5" }}, "5 health."},					//21

			{ "Twig", SpriteBase(376), FOLLOW_HAND_TEXTURE,  VALUE, { { TOOL_ACTION::VALUE, "1" } }, "Snaps easily."},					//22
			{ "Cloth", SpriteBase(377), FOLLOW_HAND_TEXTURE,  VALUE, { { TOOL_ACTION::VALUE, "3" } }, "Soft and stretchy."},			//23

			{ "Wet Washcloth", SpriteBase(378), FOLLOW_HAND_TEXTURE, WEAPON, { { TOOL_ACTION::WEAPON, "4,0,0.2,20" } }, "Slaps really hard."},			//24

			{ "Small Stone", SpriteBase(379), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "2" } }, "Made of rock."},			//25

			{ "Save Point", SpriteBase(286), FOLLOW_HAND_TEXTURE, PLACEABLE, { { TOOL_ACTION::PLACEABLE, "21,32" } }, "Can be placed to save progress."},			//26

			{ "Screw", SpriteBase(380), NO_HELD_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "5" } }, "Holds stuff together."},			//27

			{ "Bomb", SpriteBase(381), FOLLOW_HAND_TEXTURE, RANGED_WEAPON | VALUE, { { TOOL_ACTION::RANGED_WEAPON, "35,0,0.35,150,0" }, { TOOL_ACTION::VALUE, "20" } }, "Explode big."},					//28
			{ "Grenade", SpriteBase(383), FOLLOW_HAND_TEXTURE, RANGED_WEAPON | VALUE, { { TOOL_ACTION::RANGED_WEAPON, "40,0,0.35,80,1" }, { TOOL_ACTION::VALUE, "20" } }, "Dont forget the pin."},		//29
			{ "C4", SpriteBase(384), FOLLOW_HAND_TEXTURE, RANGED_WEAPON | VALUE, { { TOOL_ACTION::RANGED_WEAPON, "80,0,0.35,210,2" }, { TOOL_ACTION::VALUE, "35" } }, "Explode really big."},			//30
		};


		/// <summary>
		/// Returns the corresponding texture for the given item type.
		/// </summary>
		/// <param name="itemType">The type of item to get the texture of.</param>
		/// <returns>The texture relating to the given item type.</returns>
		inline Sprite* const ITEMTYPE_GetItemTypeTexture(ITEM_TYPE itemType)
		{
			return Renderer::GetSprite(ITEM_DATA[itemType].Texture);
		}

		/// <summary>
		/// Returns the corresponding name for the given item type. 
		/// </summary>
		/// <param name="itemType">The type of item to get the name of.</param>
		/// <returns>The display name relating to the given item type.</returns>
		inline const std::string ITEMTYPE_GetItemTypeName(ITEM_TYPE itemType)
		{
			return ITEM_DATA[itemType].DisplayName;
		}

		/// <summary>
		/// Returns a const reference to the data of the given item type.
		/// </summary>
		/// <param name="itemType">The type of item to get the data of.</param>
		/// <returns>A const reference to the data relating to the given item type.</returns>
		inline const ItemData& ITEMTYPE_GetItemData(ITEM_TYPE itemType)
		{
			return ITEM_DATA[itemType];
		}

		
	}
}