#include "BikeVehicle.h"

namespace GAME_NAME::Objects::Vehicles
{	


	BikeVehicle::BikeVehicle(Vec2 position)
		: Vehicle(position, 
			VehicleData {
				Player::Player::PLAYER_ANIMATION_STATE::BIKING,	//Animation
				Vec2{ 26.5f, 18.f },							//Scale
				SpriteBase(264),								//Sprite
			    Vec2{ 300.f, 0.f }								//Speed
			})
	{
	}

}

