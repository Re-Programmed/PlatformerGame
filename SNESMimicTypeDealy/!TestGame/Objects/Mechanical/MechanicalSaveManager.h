#pragma once

#include "../../../Resources/Save/SaveManager.h"
#include "../../../Objects/MiscStateGroup.h"

#include "../../../Objects/GameObject.h"

namespace GAME_NAME::Objects::Mechanical
{
	class MechanicalSaveManager
		: public MiscStateGroup
	{
	public:
		/// <summary>
		/// Types of mechanical objects that can be saved and loaded.
		/// </summary>
		enum class MechanicalObjectId
		{
			SMALL_COG = 0,
			LARGE_COG = 1,
			BELT = 2,
			TRIGGER_COG = 3,
			TRIGGER_LEVER = 4,
			TRIGGER_KEY_HOLE = 5
		};
		
		/// <summary>
		/// Get the mechanical object id based on the class of MiscState.
		/// </summary>
		/// <param name="object"></param>
		/// <returns></returns>
		static MechanicalObjectId GetObjectIdFromType(MiscState* object);

		static MechanicalSaveManager* INSTANCE;
		
		/// <summary>
		/// Create a load all the save data.
		/// </summary>
		static inline void Init()
		{
			if (INSTANCE != nullptr) { return; }

			//Creates and sets up a new save manager.
			new MechanicalSaveManager();
		}

		/// <summary>
		/// Stores information on a saved mechanical component.
		/// </summary>
		class MechanicalSave
			: public MiscState
		{
		public:
			std::string Level;
			size_t ObjectIndex;
			char ID;
			MiscState::SaveParam Data;
			GameObject* Object;

			MechanicalSave(std::string level, char id, size_t objectIndex, MiscState::SaveParam data, GameObject* object = nullptr)
				: Level(level), ID(id), ObjectIndex(objectIndex), Data(data), Object(object)
			{

			}

			SaveParam Encode();
			void Decode(const SaveParam param);
		};

		MechanicalSaveManager();

		static void LoadAllMechanicalObjectsForCurrentLevel();

		static std::vector<MechanicalSave*> GetSavesForLevel(std::string level);

		static MiscState* CreateObjectFromMechanicalSave(MechanicalSave save);
		
		/// <summary>
		/// Should be called every time a mechanical object is updated and needs to be saved or when the object is loaded in order to detect the correct state.
		/// </summary>
		/// <param name="object"></param>
		static void UpdateObject(MiscState* object);
		static void UnregisterObject(GameObject* object);

		static inline void ClearObjectRefs()
		{
			for (MechanicalSave*& save : m_saves)
			{
				save->Object = nullptr;
			}
		}

	private:
		/// <summary>
		/// Returns just the level name part of the current save destination.
		/// </summary>
		/// <returns></returns>
		static std::string getCurrentLevelSaveId();

		/// <summary>
		/// Gets the mechanical object save id for a given position. (Allows existing mechanical objects to overwrite each other based on position).
		/// </summary>
		/// <param name="position"></param>
		/// <returns></returns>
		static size_t getObjectId(Vec2 position);

		static std::vector<MechanicalSave*> m_saves;
	};

}