#pragma once
#include "../../Rendering/Sprite.h"
#include "../../Rendering/Renderers/Renderer.h"

#include <unordered_map>

#include "../../Audio/SoundEvents.h"
#include "../../Resources/AssetManager.h"

#define ITEM_PREFIX_ITEM 'i'
#define ITEM_PREFIX_TOOL 't'
#define ITEM_PREFIX_WEAPON 'w'

namespace GAME_NAME
{
	namespace Items
	{
#define ITEM_COUNT 65
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
			C4 = 30,
			LOG_SHIELD = 31,
			WHITE_BREAD = 32,
			GLUTEN_FREE_BREAD = 33,
			SOURDOUGH_BREAD = 34,
			SCREWDRIVER = 35,
			SMALL_WOODEN_COG = 36,
			LARGE_WOODEN_COG = 37,
			KEY = 38,
			BUTTER = 39,
			IRON_ROD = 40,
			COPPER_WIRE = 41,
			RESISTOR = 42,
			CAPACITOR = 43,
			PEAR = 44,
			ORANGE = 45,
			SPRING_BOOTS = 46,
			SPRING = 47,
			LAUNCHPAD = 48,
			WRENCH = 49,
			PITCHFORK = 50,
			BUCKET = 51,
			MILK_JUG = 52,
			WHEAT = 53,
			PICKAXE = 54,
			PISTOL = 55,
			PISTOL_AMMO = 56,
			BOW = 57,
			COMPOUND_BOW = 58,
			ARROW = 59,
			COPPER_PLATE = 60,
			IRON_PLATE = 61,
			TANTALUM_PLATE = 62,
			BRASS_PLATE = 63,
			WOODEN_BENCH = 64
		};

		/// <summary>
		/// Different actions that tools can perform. Can be checked for when determining wether the player can perform some action based on what tool they are holding.
		/// </summary>
		enum TOOL_ACTION
		{
			CHOP =			0b000000000001, //Can cut down trees.
			EQUIPMENT =		0b000000000010, //Can be equipped to equipment slots.
			MINE =			0b000000000100, //Can break BreakableBlocks. (provide a strength attribute)
			WEAPON =		0b000000001000, //Can be used to attack. (provide a damage, power consumption, reload attribute, area of effect, and sound [damage,powerconsume,reloadseconds,AOE(integer),sound=Punch])
			FOOD =			0b000000010000, //Can be eaten.
			PLACEABLE =		0b000000100000, //Can be placed.
			FURNITURE =		0b000001000000, //Furniture. (xScale, yScale, inventorySize[optional], isOutdoor[optional])
			VALUE =			0b000010000000, //Give an item a specific price value. (crumbValue)
			RANGED_WEAPON = 0b000100000000, //Shoots projectiles [damage,powerconsume,reloadseconds,range,projectile_type]
			USE_SOUND =		0b001000000000, //Plays a different sound when used. [soundEventNumber]
			SHIELD =		0b010000000000, //Blocks damage [resistance,recoveryTime]
			FIREARM =		0b100000000000	//Shoots bullet [clip_size,clip_itemID,damage,reload_time,shot_interval]
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
			mutable std::string DisplayName;
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
			const uint32_t Actions = 0;
			/// <summary>
			/// Stores data about the items actions like its strength.
			/// </summary>
			const std::unordered_map<TOOL_ACTION, std::string> Attributes;
			/// <summary>
			/// Whatever you want.
			/// </summary>
			mutable std::string Description;
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
			{ "Axe", SpriteBase(65), SpriteBase(65), WEAPON | CHOP, {{TOOL_ACTION::CHOP , "1"}, {TOOL_ACTION::WEAPON, "4,0,0.9,20"}}, ""},	//3
			{ "Unknown", 0, NO_HELD_TEXTURE, 0, {}, "" },																	//4
			{ "Wooden Shoes", SpriteBase(453), NO_HELD_TEXTURE, VALUE | EQUIPMENT, { {TOOL_ACTION::VALUE, "10" }, { TOOL_ACTION::EQUIPMENT, "Health:8" }}, ""},			//5
			{ "Sharp Stick", SpriteBase(118), SpriteBase(117), WEAPON, {{TOOL_ACTION::WEAPON, "3,0,0.5,26"}}, ""},			//6
			{ "Toast", SpriteBase(132), SpriteBase(233), 0, {}, "Probably made in a toaster." },							//7
			{ "Apple", SpriteBase(247), NO_HELD_TEXTURE, FOOD, { { TOOL_ACTION::FOOD, "6" } }, "Apple."},					//8
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
			{ "Spear", SpriteBase(298), FOLLOW_HAND_TEXTURE, WEAPON | VALUE, { { TOOL_ACTION::WEAPON, std::string("6,0,1.2,45,").append(std::to_string(static_cast<int>(Audio::SoundEvents::Event::HIT_SLICED))) }, { TOOL_ACTION::VALUE, "30" }}, "Reach really far."},					//19

			{ "Wooden Plating", SpriteBase(374), NO_HELD_TEXTURE, EQUIPMENT | VALUE, { { TOOL_ACTION::EQUIPMENT, "Health:15" }, { TOOL_ACTION::VALUE, "20" }}, "15 health."},					//20
			{ "Leaf Helmet", SpriteBase(375), NO_HELD_TEXTURE, EQUIPMENT | VALUE, { { TOOL_ACTION::EQUIPMENT, "Health:5" }, { TOOL_ACTION::VALUE, "5" }}, "5 health."},					//21

			{ "Twig", SpriteBase(376), FOLLOW_HAND_TEXTURE,  VALUE, { { TOOL_ACTION::VALUE, "1" } }, "Snaps easily."},					//22
			{ "Cloth", SpriteBase(377), FOLLOW_HAND_TEXTURE,  VALUE, { { TOOL_ACTION::VALUE, "3" } }, "Soft and stretchy."},			//23

			{ "Wet Washcloth", SpriteBase(378), FOLLOW_HAND_TEXTURE, WEAPON, { { TOOL_ACTION::WEAPON, "4,0,0.2,20" } }, "Slaps really hard."},			//24

			{ "Small Stone", SpriteBase(379), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "2" } }, "Made of rock."},			//25

			{ "Save Point", SpriteBase(286), FOLLOW_HAND_TEXTURE, PLACEABLE, { { TOOL_ACTION::PLACEABLE, "21,32" } }, "Can be placed to save progress."},			//26

			{ "Screw", SpriteBase(380), NO_HELD_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "5" } }, "Holds stuff together."},			//27

			{ "Bomb", SpriteBase(381), FOLLOW_HAND_TEXTURE, RANGED_WEAPON | VALUE, { { TOOL_ACTION::RANGED_WEAPON, "35,0,0.35,150,0" }, { TOOL_ACTION::VALUE, "20" } }, "Explode big."},				//28
			{ "Grenade", SpriteBase(383), FOLLOW_HAND_TEXTURE, RANGED_WEAPON | VALUE, { { TOOL_ACTION::RANGED_WEAPON, "40,0,0.35,80,1" }, { TOOL_ACTION::VALUE, "20" } }, "Dont forget the pin."},		//29
			{ "C4", SpriteBase(384), FOLLOW_HAND_TEXTURE, RANGED_WEAPON | VALUE, { { TOOL_ACTION::RANGED_WEAPON, "80,0,0.35,210,2" }, { TOOL_ACTION::VALUE, "35" } }, "Explode really big."},			//30
			{ "Log Shield", SpriteBase(385), FOLLOW_HAND_TEXTURE, SHIELD | VALUE, { { TOOL_ACTION::SHIELD, "4,0.75" }, { TOOL_ACTION::VALUE, "7" } }, "Might block some weapons?"},						//31

			{ "White Bread", SpriteBase(386), FOLLOW_HAND_TEXTURE, 0, { }, "Hyper gluten injected."},						//32
			{ "Gluten Free Bread", SpriteBase(387), FOLLOW_HAND_TEXTURE, 0, { }, "Almonds?"},		//33
			{ "Sourdough Bread", SpriteBase(388), FOLLOW_HAND_TEXTURE, 0, { }, "Flavorful."},								//34

			{ "Screwdriver", SpriteBase(430), FOLLOW_HAND_TEXTURE, VALUE | WEAPON, { { TOOL_ACTION::WEAPON, "3,0,0.4,26" }, { TOOL_ACTION::VALUE, "3"  } }, "Has a magnetic tip."},								//35

			{ "Small Cog", SpriteBase(441), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "3"  } }, "RMB to remove, LMB to place next to cogs."},							//36
			{ "Large Cog", SpriteBase(442), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "5"  } }, "RMB to remove, LMB to place next to cogs."},							//37

			{ "Key", SpriteBase(444), FOLLOW_HAND_TEXTURE, 0, { }, "Unlocks stuff."},																							//38

			{ "Butter", SpriteBase(445), FOLLOW_HAND_TEXTURE, VALUE | RANGED_WEAPON, { { TOOL_ACTION::RANGED_WEAPON, "4,0,0.35,80,3" }, { TOOL_ACTION::VALUE, "3" } }, "Thowable.\nMakes things slippery."},			//39

			{ "Iron Rod", SpriteBase(447), FOLLOW_HAND_TEXTURE, VALUE | WEAPON, { { TOOL_ACTION::VALUE, "5" }, {TOOL_ACTION::WEAPON, "3,0,0.5,26"} }, "Very structural."},			//40

			{ "Copper Wire", SpriteBase(448), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "6" } }, "Simple conductive material."},			//41
			{ "Resistor", SpriteBase(449), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "12" } }, "Uncalled for\nelectron gatekeeping."},			//42
			{ "Capacitor", SpriteBase(450), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "12" } }, "0.1 uF."},			//43

			{ "Pear", SpriteBase(451), FOLLOW_HAND_TEXTURE, VALUE | FOOD, { { TOOL_ACTION::VALUE, "2" }, { TOOL_ACTION::FOOD, "10"} }, "Pears grow on pear trees."},			//44
			{ "Orange", SpriteBase(452), FOLLOW_HAND_TEXTURE, VALUE | FOOD, { { TOOL_ACTION::VALUE, "2" }, { TOOL_ACTION::FOOD, "10"} }, "Edible."},			//45
			
			{ "Spring Boots", SpriteBase(454), FOLLOW_HAND_TEXTURE, VALUE | EQUIPMENT, { { TOOL_ACTION::VALUE, "24" }, { TOOL_ACTION::EQUIPMENT, "Health:8,Jump:10"} }, "Jump higher."},			//46

			{ "Spring", SpriteBase(455), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "4" } }, "Bouncy."},			//47

			{ "Launchpad", SpriteBase(456), NO_HELD_TEXTURE, VALUE | PLACEABLE, { { TOOL_ACTION::VALUE, "12" }, { TOOL_ACTION::PLACEABLE, "24,24" }}, "Place to get launched."},			//48

			{ "Wrench", SpriteBase(458), FOLLOW_HAND_TEXTURE, VALUE | WEAPON, { { TOOL_ACTION::WEAPON, "3,0,0.6,26" }, { TOOL_ACTION::VALUE, "8" } }, "Works on all screws."},			//49

			{ "Pitchfork", SpriteBase(459), FOLLOW_HAND_TEXTURE, VALUE | WEAPON, { { TOOL_ACTION::WEAPON, std::string("5,0,1.1,36,").append(std::to_string(static_cast<int>(Audio::SoundEvents::Event::HIT_SLICED))) }, {TOOL_ACTION::VALUE, "6"}}, "Stabs far away."},			//50
			
			{ "Bucket", SpriteBase(460), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "5" } }, "Holds whatever you want."},			//51

			{ "Milk Jug", SpriteBase(461), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "8" } }, "Holds milk."},			//52

			{ "Wheat", SpriteBase(462), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "1" } }, "Almost bread?"},			//53

			{ "Pickaxe", SpriteBase(463), FOLLOW_HAND_TEXTURE, VALUE | MINE, { { TOOL_ACTION::VALUE, "30" }, { TOOL_ACTION::MINE, "3" } }, "Mines certain resources.\nCan clear certain pathways."},			//54

			{ "Pistol", SpriteBase(463), FOLLOW_HAND_TEXTURE, VALUE | FIREARM, { { TOOL_ACTION::VALUE, "100" }, { TOOL_ACTION::FIREARM, "5,38,20,1.5,0.5" } }, "Shoots."},			//55
			{ "Pistol Ammo", SpriteBase(455), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "15" } }, "Not that painful\nat low speed."},			//56

			{ "Bow", SpriteBase(465), FOLLOW_HAND_TEXTURE, VALUE | FIREARM, { { TOOL_ACTION::VALUE, "45" }, { TOOL_ACTION::FIREARM, "8,59,10,2.0,1.0" } }, "Unstackable."},			//57
			{ "Compound Bow", SpriteBase(466), FOLLOW_HAND_TEXTURE, VALUE | FIREARM, { { TOOL_ACTION::VALUE, "60" }, { TOOL_ACTION::FIREARM, "8,59,15,1.5,0.8" } }, "I'll have a Bow.\nWheels on the side."},			//58
			{ "Arrows", SpriteBase(468), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "8" } }, "Eight arrows." },			//59

			{ "Copper Plate", SpriteBase(469), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "15" } }, "Malleable." },									//60
			{ "Iron Plate", SpriteBase(470), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "20" } }, "Sounds cool if\nyou smack it." },					//61
			{ "Tantalum Plate", SpriteBase(471), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "23" } }, "I think there is\ntantalum in my pants." },		//62
			{ "Brass Plate", SpriteBase(472), FOLLOW_HAND_TEXTURE, VALUE, { { TOOL_ACTION::VALUE, "25" } }, "Someone got their\nzinc in my copper!" },			//63

			{ "Wooden Bench", 53, NO_HELD_TEXTURE, FURNITURE | VALUE, {{TOOL_ACTION::FURNITURE, "28,16.5.5,0,1"}, {TOOL_ACTION::VALUE, "15"}}, "Fits 10 people."},					//64
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

		static void ITEMTYPE_LoadItemTranslations()
		{
			std::unordered_map<std::string, Resources::AssetManager::ItemInformation> itemLanguageData = Resources::AssetManager::GetItemLanguageData();

			for (int i = 0; i < ITEM_COUNT; i++)
			{
				std::string itemName = ITEMTYPE_GetItemTypeName(static_cast<ITEM_TYPE>(i));
				if (itemLanguageData.contains(itemName))
				{
					ITEM_DATA[i].DisplayName = itemLanguageData[itemName].Name;
					ITEM_DATA[i].Description = itemLanguageData[itemName].Description;
				}
			}
		};
	}
}