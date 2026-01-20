#pragma once

#include "Vehicle.h"

namespace GAME_NAME::Objects::Vehicles
{
	class BikeVehicle
		: public Vehicle
	{
	public:
		BikeVehicle(Vec2 position);
	};
}