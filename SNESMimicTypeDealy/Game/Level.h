#pragma once
#include "../Audio/SoundManager.h"
#include "../Utils/Math/Vec2.h"
#include "../Utils/Math/Vec3.h"
#include <set>

#define LEVEL_NO_PLAYER_FLAG "NO_PLAYER"
#define LEVEL_NO_PLAYER_INTRO_FLAG "NO_PLAYER_INTRO"
#define LEVEL_ROOM_CONTROLS_FLAG "ROOM_CONTROLS"
#define LEVEL_FEET_ONLY_COLLISION_FLAG "PLAYER_FEET_ONLY_COLLISION"


namespace GAME_NAME
{
	namespace Game
	{
		using namespace MathUtils;

		struct Level	//Stores basic data about levels.
		{
			/// <summary>
			/// Name/Path of the level.
			/// </summary>
			std::string Name, Path;
			/// <summary>
			/// World and level ID.
			/// </summary>
			struct { int8_t World, Level; } ID;

			/// <summary>
			/// The voice of the current background track.
			/// </summary>
			Audio::AudioVoice CurrentBGMusic;

			Vec2 PlayerStartPosition = Vec2::Zero;

			Vec3 BackgroundColor = Vec3(0,0,0);

			/// <summary>
			/// Flags that the level has set. Can be used to modify what happens on level load in the Game class.
			/// Usable flags for TEST_GAME:
			///		NO_PLAYER
			/// </summary>
			std::set<std::string> Flags;
		};
	}
}