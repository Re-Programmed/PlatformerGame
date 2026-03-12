#pragma once

#ifndef _COG_CLASS
#define _COG_CLASS

#include "../../../Objects/GameObject.h"
#include "../../../Rendering/DynamicSprite.h"
#include "./Belt.h"


namespace GAME_NAME::Objects::Mechanical
{
	class Belt;

	class Cog
		: public GameObject
	{
	public:
		/// <summary>
		/// Relates the larger and smaller cog sprites with each other. (small, large).
		/// </summary>
		static std::unordered_map<int, const int> CogSpriteReference;

		enum class CogConnection
		{
			Up,
			Down,
			Left,
			Right,
			Up_Left,
			Up_Right,
			Down_Left,
			Down_Right
		};

		static CogConnection OppositeConnection(const CogConnection& connection);

		Cog(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, bool counterClockwise = false, uint8_t gearRatio = 1, float initalRotationOffset = 0.f, bool isRotationSource = false);
		~Cog();
		/// <summary>
		/// Call this to check the mouse and update the display based on if the player is trying to place a cog.
		/// </summary>
		static void DetectPlacingCog(uint8_t gearRatio);
		static void HidePlacingCog();

		static inline void ClearCogRegistry()
		{
			m_cogs.clear();
		}

		void Update(GLFWwindow* window) override;

		void Render(const Vec2& cameraPosition) override;

		void DestroyAndUpdateAdjacent();

		Vec2 GetAdjacentCogOffset(CogConnection direction);

		Cog* CreateAdjacentCog(CogConnection direction);

		/// <summary>
		/// How two cogs are related to each other (even if not connected).
		/// </summary>
		struct CogInteraction
		{
			bool IsOverlap = true;
			bool Distant = false;
			CogConnection Connected = CogConnection::Right;
		};

		CogInteraction GetInteraction(Cog* otherCog);
		CogInteraction GetInteraction(Vec2 bottomLeft, Vec2 scale, uint8_t gearRatio);

		void SetBelted(Belt* belt);

		/// <summary>
		/// Returns true if the specified point and scale overlaps a cog beyond the teeth.
		/// </summary>
		/// <returns></returns>
		static std::vector<std::tuple<CogInteraction, Cog*>> GetInteractingCogs(Vec2 bottomLeft, Vec2 scale, uint8_t gearRatio);
		static std::vector<std::tuple<CogInteraction, Cog*>> GetInteractingCogs(Cog* cog);

		/// <summary>
		/// Get all connected cogs.
		/// </summary>
		/// <returns></returns>
		const std::vector<std::tuple<Cog*, CogConnection>>& GetConnections()
		{
			return m_connections;
		}


		inline bool IsCounterClockwise() { return m_counterClockwise; }

		inline const int& GetGearRatio() { return m_gearRatio; }
		inline const float& GetGearSpeed() { return m_gearSpeed; }

	protected:
		Belt* m_attachedBelt = nullptr;

		std::vector<std::tuple<Cog*, CogConnection>> m_connections;

		static std::vector<Cog*> m_cogs;

		typedef struct DisplayCog {
			std::unique_ptr<Rendering::DynamicSprite> Sprite = nullptr;
			Cog* ReferenceCog = nullptr;
			CogConnection Direction = CogConnection::Right;

			float DisplayRotation = 0.f;
			Vec2 DisplayOffset = Vec2{ 0.f, 0.f };
		};

		/// <summary>
		/// Adds a connection to this cog, does not check that the connection is viable.
		/// </summary>
		/// <param name="cog"></param>
		void addConnection(Cog* cog, CogConnection direction)
		{
			m_connections.emplace_back(cog, direction);
		}

		/// <summary>
		/// Removes the specified cog from being a connection to this cog. If the cog is not found, returns false.
		/// </summary>
		/// <param name="cog"></param>
		/// <returns></returns>
		bool removeConnection(Cog* cog);
		bool removeConnection(CogConnection direction);

		/// <summary>
		/// Used for showing where a cog will be placed when connecting new cogs.
		/// </summary>
		static DisplayCog m_displayCog;

		static void displayCogPlaceGraphic(Cog* cog, CogConnection direction);
		static double m_cogPlaceTimer;
		static double m_cogRemoveTimer;

		bool m_counterClockwise;
		/// <summary>
		/// Will affect the speed of rotation for various gears connected.
		/// </summary>
		const uint8_t m_gearRatio;
		//How fast the gear is spinning.
		float m_gearSpeed;

		/// <summary>
		/// This gear always rotates and will push other gears.
		/// </summary>
		const bool m_isRotationSource;

	private:
		/// <summary>
		/// True if this gear is connected to a rotation source or another gear that is connected to a rotation source.
		/// Used to determine if a gear's connection to a source has been severed.
		/// If this ever becomes false, the gear will stop.
		/// </summary>
		bool m_isRecievingRotationPower = false;
	};
}
#endif