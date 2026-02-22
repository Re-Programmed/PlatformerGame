#pragma once

#include "../../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"

#include "../../Items/ItemType.h"
#include "../../Items/InventoryItem.h"

#include "../../../Objects/GameObjectState.h"

#include "./Effects/ExplosionEffectedObject.h"

namespace GAME_NAME::Objects
{
	using namespace Components::Physics::Collision;


	class BreakableBlock
		: public StaticBoxCollisionObject, public GameObjectState, public Environment::Effects::ExplosionEffectedObject
	{
	public:
		BreakableBlock(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, double mineTime = 1.0, int mineResistance = 0, Items::TOOL_ACTION requiredActionFlag = Items::TOOL_ACTION::MINE, Items::InventoryItem* drops = nullptr);

		void Render(const Vec2& cameraPosition) override;

		void Update(GLFWwindow* window) override;

		void Exploded(Vec2 origin, float power) override;
		
		/* Save data stuff. */
		void SaveState() override;
		void LoadState() override;
	protected:
		/// <summary>
		/// Breaks this block (creates particles and removes the block)
		/// </summary>
		/// <param name="loadingFromSave">[bool] - If true, the block will be deleted from the game since it will never be used.</param>
		void breakBlock(bool loadingFromSave = false);

		bool m_isBroken;

	private:
		Items::InventoryItem* m_drops = nullptr;

		Vec2 m_shakeOffset; 
		double m_shakeTimer = 0.0;

		/// <summary>
		/// True if the block is currently hovered for mining.
		/// </summary>
		bool m_isHovered = false;

		/// <summary>
		/// How long it takes to break this object.
		/// </summary>
		const double m_mineTime;
		/// <summary>
		/// The minimum tool strength required to break this object.
		/// </summary>
		const int m_mineResistance;
		/// <summary>
		/// The flags an item must have in order to mine this block.
		/// </summary>
		const Items::TOOL_ACTION m_requiredActionFlag;

		/// <summary>
		/// Used to fade in and out the hover outline.
		/// </summary>
		double m_hoverDisplayPercent = 0.0;

		/// <summary>
		/// How long the user has been mining a block for.
		/// </summary>
		static double m_currentMiningTime;
	};
}