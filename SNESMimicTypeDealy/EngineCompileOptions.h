#pragma once

/*
	This file contains settings that you will need to change based on what features you use in the engine and where you put certain files.
*/

#define SHOW_WINDOWS_ERROR_POPUPS

#pragma region Renderer	
	
	/*
	---RENDER ORDER---

	   [BACKGROUND] {Static.}
	   [RENDER LAYER 0] {Bound to the chunk spawned in.}
	   [RENDER LAYER 1]	|
	   [RENDER LAYER 2]	|
	   [RENDER LAYER 3]	|
	   [ACTIVE RENDER LAYER 0] {Rendered if visible by camera.}
	   [ACTIVE RENDER LAYER 1]		|
	   [ACTIVE RENDER LAYER 2]		|	(PLAYER)
	   [ACTIVE RENDER LAYER 3]		|
	   [PRIORITY OBJECTS (FRONT OBJECTS)] {Bound to the chunk spawned in.}
	   [GUI LAYER 0] {Always rendered.}
	   [GUI LAYER 1]	|
	   [GUI LAYER 2]	|
	*/

	#define GLOBAL_SPRITE_BASE -263								//Represents the negative number of sprites in the global_sprites directory.

	#define DEFAULT_LEVEL_SIZE_X 240							//The default size of a level in chunks.
	#define DEFAULT_LEVEL_SIZE_Y 5								//The default size of a level in chunks.

#pragma endregion

#pragma region TextRenderer	

	#define TEXT_RENDERER_ZERO_DIGIT_SPRITE_ID SpriteBase(55)	//The texture used for the digit 0. All subsequent digits should be after it in ascending order.
	#define TEXT_RENDERER_DIGIT_SIZE_X 20.f						//The X scale of all digits rendered by the text renderer.
	#define TEXT_RENDERER_DIGIT_SIZE_Y 24.f						//The Y scale of all digits rendered by the text renderer.

	#define DEFAULT_FONT_RENDER_A_SPRITE_ID SpriteBase(75)					//The texture used for the default font letter "a" (All subsequent digits should be after it in alphabetic order)
	#define DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID SpriteBase(157)		//The texture used for the default lowercase font letter "a" (All subsequent digits should be after it in alphabetic order)

#pragma endregion
