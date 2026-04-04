#pragma once

/*
	This file was created for the purpose of adding custom defines that will be used during compiling.
*/

#define DIALOGUE_MANAGER_GLOBAL_DIALOGUE_FOLDER "/global_assets"

#pragma region InventoryData

	#define INVENTORY_UNSELECTED_SLOT_SPRITE SpriteBase(53)	//The sprite of an inventory slot.
	#define INVENTORY_SELECTED_SLOT_SPRITE SpriteBase(54)	//The sprite of a selected inventory slot.

#pragma endregion

#pragma region Door

	#define  DEFAULT_DOOR_OPEN_DISTANCE 9.5f	//The default minimum distance requrired for the player to be from a door for it to open or close.
	
#pragma endregion