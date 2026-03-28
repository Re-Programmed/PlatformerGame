#include "MechanicalSaveManager.h"
#include "./TriggerKeyHole.h"
#include "./TriggerLever.h"

#include "../../../Resources/Save/SaveManager.h"

namespace GAME_NAME::Objects::Mechanical
{
	std::vector<MechanicalSaveManager::MechanicalSave*> MechanicalSaveManager::m_saves;

	MechanicalSaveManager* MechanicalSaveManager::INSTANCE = nullptr;

	MechanicalSaveManager::MechanicalSaveManager()
		: MiscStateGroup("mc")
	{
		if (INSTANCE != nullptr)
		{
			return;
		}

		auto states = getStates();

		//Data format: level,id,data
		for (std::string& data : *(states.get()))
		{
			MechanicalSave* save = new MechanicalSave("", 0, 0, "");
			save->Decode(data);
			m_saves.push_back(save);

			assignState(m_saves[m_saves.size() - 1]);
		}

		INSTANCE = this;
	}

	MiscState::SaveParam MechanicalSaveManager::MechanicalSave::Encode()
	{
		SaveParam param;
		param.append(this->Level).append(";")
			.append(std::to_string(this->ObjectIndex)).append(";")
			.append(std::to_string(this->ID)).append(";")
			.append(this->Data);

		return param;
	}

	void MechanicalSaveManager::MechanicalSave::Decode(const MiscState::SaveParam param)
	{
		std::stringstream ss(param);
		std::string line;

		int i = 0;
		while (std::getline(ss, line, ';'))
		{
			switch (i++)
			{
			case 0:
				this->Level = line;
				break;
			case 1:
				this->ObjectIndex = std::stoll(line);
				break;
			case 2:
				this->ID = std::stoi(line);
				break;
			case 3:
				this->Data = line;
				break;
			}
		}
	}

	void MechanicalSaveManager::LoadAllMechanicalObjectsForCurrentLevel()
	{
		std::string level = getCurrentLevelSaveId();

		for (MechanicalSave* save : GetSavesForLevel(level))
		{
			CreateObjectFromMechanicalSave(*save);
		}
	}

	std::vector<MechanicalSaveManager::MechanicalSave*> MechanicalSaveManager::GetSavesForLevel(std::string level)
	{
		std::vector<MechanicalSave*> saves;

		for (MechanicalSave*& save : m_saves)
		{
			if (save->Level == level)
			{
				saves.push_back(save);
			}
		}

		return saves;
	}

	MiscState* MechanicalSaveManager::CreateObjectFromMechanicalSave(MechanicalSave save)
	{
		switch (save.ID)
		{
		case static_cast<int>(MechanicalObjectId::TRIGGER_KEY_HOLE):
		case static_cast<int>(MechanicalObjectId::TRIGGER_LEVER):
		{
			MiscState* state = dynamic_cast<MiscState*>(save.Object);
			if (state != nullptr)
			{
				state->Decode(save.Data);
			}

			break;
		}

		default:
			return nullptr;
		}
	}

	void MechanicalSaveManager::UpdateObject(MiscState* object)
	{
		size_t objectIndex = 0;

		GameObject* go = dynamic_cast<GameObject*>(object);

		if (go == nullptr) { return; }

		//Object index is represented by the position of the object but combined.
		objectIndex = getObjectId(go->GetPosition());		

		//TODO: Should probably be a better searching algorithm to not make super slow?
		for (MechanicalSave*& save : m_saves)
		{
			if (save->ObjectIndex == objectIndex)
			{
				//If the object is already saved but has no related object, this means that we just tried to load the object from the level data after we had already loaded the previous actual save data.
				//This allows us to cross reference the object and its data.
				if (save->Object == nullptr)
				{
					save->Object = go;
				}
				else {
					if (dynamic_cast<MiscState*>(save->Object) == object)
					{
						save->Data = object->Encode();
					}
				}

				return;
			}
		}

		//No data exists yet.
		m_saves.push_back(new MechanicalSave(getCurrentLevelSaveId(), static_cast<char>(GetObjectIdFromType(object)), objectIndex, object->Encode(), go));
		INSTANCE->assignState(m_saves[m_saves.size() - 1]);
	}

	void MechanicalSaveManager::UnregisterObject(GameObject* object)
	{
		int objectIndex = getObjectId(object->GetPosition());

		for (int i = 0; i < m_saves.size(); i++)
		{
			if (m_saves[i]->ObjectIndex == objectIndex)
			{
				m_saves[i]->Object = nullptr;
;				return;
			}
		}
	}

	std::string MechanicalSaveManager::getCurrentLevelSaveId()
	{
		std::string currentFile = Resources::SaveManager::GetCurrentLevelFile();
		return currentFile.substr(currentFile.find_first_of('/') + 1);
	}

	size_t MechanicalSaveManager::getObjectId(Vec2 position)
	{
		size_t objectIndex = (int)(position.X);
		objectIndex *= 1000000000000;
		objectIndex += (int)(position.Y);
		return objectIndex;
	}

	inline MechanicalSaveManager::MechanicalObjectId MechanicalSaveManager::GetObjectIdFromType(MiscState* state)
	{
		if (dynamic_cast<TriggerKeyHole*>(state))
		{
			return MechanicalObjectId::TRIGGER_KEY_HOLE;
		}

		if (dynamic_cast<TriggerLever*>(state))
		{
			return MechanicalObjectId::TRIGGER_LEVER;
		}

		return MechanicalObjectId::SMALL_COG;
	}
}
