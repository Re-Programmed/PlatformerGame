#pragma once
#include "../InventoryItem.h"
#include "../../../Objects/MiscState.h"

#define ITEM_PREFIX_FIREARM 'g'

namespace GAME_NAME::Items
{
	class Firearm
		: public InventoryItem
	{
	public:
		Firearm(ITEM_TYPE type);
		Firearm();

		bool Shoot(GameObject* shooter, Vec2 origin);

		const double& Reload();

		struct FirearmStats
		{
			ITEM_TYPE Clip = ITEM_TYPE::NULL_ITEM;
			float Damage = 0.f;
			double ShotTime = 0.0;
			double ReloadTime = 0.0;
			unsigned int ClipSize = 0;
		};

		SaveParam Encode() override;
		void Decode(const SaveParam params) override;

		static const FirearmStats GetStats(std::string data);
		static const FirearmStats GetStats(ITEM_TYPE firearm);

		/// <summary>
		/// Creates a shot and returns the hit entity. Damages what is hit and creates an effect.
		/// </summary>
		/// <param name="origin"></param>
		/// <param name="left"></param>
		/// <param name="shooter"></param>
		/// <returns></returns>
		static GameObject* CreateShot(Vec2 origin, bool left, GameObject* shooter = nullptr);

		inline const int GetAmmo() { return m_ammo; }

	protected:
		inline constexpr const char getPrefix() override { return ITEM_PREFIX_FIREARM; }

	private:
		unsigned int m_ammo = 0;
	};
}