#include "Firearm.h"

#include <sstream>

#include "../../Objects/Environment/Effects/FlashEffect.h"
#include "../../Objects/Environment/Effects/Explosion.h"
#include "../../../Utils/Math/Ray.h"

#include "../../TestGame.h"

#define BULLET_TRAIL_SPRITE SpriteBase(464)

namespace GAME_NAME::Items
{
	Firearm::Firearm(ITEM_TYPE type)
		: InventoryItem(type)
	{

	}

	Firearm::Firearm()
		: InventoryItem()
	{
	}

	bool Firearm::Shoot(GameObject* shooter, Vec2 origin)
	{
		if (m_ammo <= 0) {
			//TODO: play empty clip sound.
			return false;
		}

		m_ammo--;

		//TODO: play shot sound.

		CreateShot(origin, !shooter->GetTextureFlipped(), shooter);
		return true;
	}

	const double& Firearm::Reload()
	{
		FirearmStats stats = GetStats(this->m_itemType);
		this->m_ammo = stats.ClipSize;

		return stats.ReloadTime;
	}

	MiscState::SaveParam Firearm::Encode()
	{
		MiscState::SaveParam param = InventoryItem::Encode();
		param.append(",").append(std::to_string(m_ammo));
		return param;
	}

	void Firearm::Decode(const SaveParam params)
	{
		std::stringstream stream(params);
		std::string line;

		uint8_t lineInd = 0;
		while (std::getline(stream, line, ','))
		{
			switch (lineInd++)
			{
			case 0:
				InventoryItem::Decode(line);
				break;
			case 1:
				m_ammo = std::stoi(line);
				break;
			}
			
		}
	}

	const Firearm::FirearmStats Firearm::GetStats(std::string data)
	{
		std::stringstream firearmStats(data);
		std::string stat;

		uint8_t line = 0;

		Firearm::FirearmStats finalStats;

		while (std::getline(firearmStats, stat, ','))
		{
			switch (line++)
			{
			case 0:
				finalStats.ClipSize = std::stoi(stat);
				break;
			case 1:
				finalStats.Clip = static_cast<ITEM_TYPE>(std::stoi(stat));
				break;
			case 2:
				finalStats.Damage = std::stof(stat);
				break;
			case 3:
				finalStats.ReloadTime = std::stod(stat);
				break;
			case 4:
				finalStats.ShotTime = std::stod(stat);
				break;

			default:
				break;
			}
		}

		return finalStats;
	}

	const Firearm::FirearmStats Firearm::GetStats(ITEM_TYPE firearm)
	{
		const ItemData& data = ITEMTYPE_GetItemData(firearm);

		if (data.Actions & TOOL_ACTION::FIREARM)
		{
			return GetStats(data.Attributes.at(TOOL_ACTION::FIREARM));
		}
		else {
			return Firearm::FirearmStats{ };
		}
	}

	GameObject* Firearm::CreateShot(Vec2 origin, bool left, GameObject* shooter)
	{
		//Muzzle flash
		Environment::Effects::FlashEffect::CreateFlash(origin, 10.f, 0.33);
		Environment::Effects::FlashEffect::CreateFlash(origin, 10.f, 0.1);

		Vec2 hitPoint(0.f);
		GameObject* hit = Ray::Cast(origin, Vec2{ left ? -1.f : 1.f, 0.f }, hitPoint, 320.f, shooter);
		
		GAME_NAME::Camera::GameCamera* gc = dynamic_cast<GAME_NAME::Camera::GameCamera*>(TestGame::INSTANCE->GetCamera());
		gc->ScreenShake(1.1f, 0.20);

		if (hit != nullptr)
		{
			//Shot graphic.
			Environment::Effects::FlashEffect::CreateFlash(origin - Vec2{ 0.f, 1.f }, Vec2{ hitPoint.X - origin.X, 2.f }, 0.125, Renderer::GetSprite(BULLET_TRAIL_SPRITE), Vec3{ 1.f, 1.f, 0.75f });

			Environment::Effects::Explosion* explosion = new Environment::Effects::Explosion(hitPoint, 9.15f);
			Renderer::InstantiateObject(Renderer::InstantiateGameObject(explosion, true, 2, false));

			return hit;
		}

		Environment::Effects::FlashEffect::CreateFlash(origin - Vec2{ 0.f, 1.f }, Vec2{ left ? -320.f : 320.f, 2.f }, 0.125, Renderer::GetSprite(BULLET_TRAIL_SPRITE), Vec3{ 1.f, 1.f, 0.75f });

		return nullptr;
	}
}
